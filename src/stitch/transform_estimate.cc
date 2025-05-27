// File: transform_estimate.cc
// Date: Wed Nov 22 15:23:42 2017 +0800
// Author: Yuxin Wu

#include "transform_estimate.hh"

#include <set>
#include <random>

#include "feature/feature.hh"
#include "feature/matcher.hh"
#include "lib/polygon.hh"
#include "lib/config.hh"
#include "lib/imgproc.hh"
#include "lib/timer.hh"
#include "match_info.hh"
using namespace std;
using namespace config;

namespace {
const int ESTIMATE_MIN_NR_MATCH = 8;
}

namespace pano {

TransformEstimation::TransformEstimation(const MatchData& m_match,
		const std::vector<Vec2D>& kp1,
		const std::vector<Vec2D>& kp2,
		const Shape2D& shape1, const Shape2D& shape2):
	match(m_match), kp1(kp1), kp2(kp2),
	shape1(shape1), shape2(shape2),
	f2_homo_coor(match.size(), 3)
{
	if (TRANS)
		transform_type = Translation;
	else if (CYLINDER)
		transform_type = Affine;
	else
		transform_type = Homo;
	int n = match.size();
	int min_initial_matches = (transform_type == Translation) ? 4 : ESTIMATE_MIN_NR_MATCH;
	if (n < min_initial_matches) return;
	REP(i, n) {
		Vec2D old = kp2[match.data[i].second];
		f2_homo_coor.at(i, 0) = old.x;
		f2_homo_coor.at(i, 1) = old.y;
		f2_homo_coor.at(i, 2) = 1;
	}
	ransac_inlier_thres = (shape1.w + shape1.h) * 0.5 / 800 * RANSAC_INLIER_THRES;
}

bool TransformEstimation::get_transform(MatchInfo* info) {
	TotalTimer tm("get_transform");
	// use Affine in cylinder mode, and Homography in normal mode
	// TODO more condidate set will require more ransac iterations
	int nr_match_used;
	if (transform_type == Translation) {
		nr_match_used = 2; // Use 2 points for translation estimation
	} else if (transform_type == Affine) {
		nr_match_used = (6 / 2) + 4; // Keep original logic: 7 points
	} else { // Homo
		nr_match_used = (8 / 2) + 4; // Keep original logic: 8 points
	}
	int nr_match = match.size();
	if (nr_match < nr_match_used)
		return false;

	vector<int> inliers;
	set<int> selected;

	int maxinlierscnt = -1;
	Homography best_transform;

	random_device rd;
	mt19937 rng(rd());

	for (int K = RANSAC_ITERATIONS; K --;) {
		inliers.clear();
		selected.clear();
		REP(_, nr_match_used) {
			int random;
			do {
				random = rng() % nr_match;
			} while (selected.find(random) != selected.end());
			selected.insert(random);
			inliers.push_back(random);
		}
		auto transform = calc_transform(inliers);
		if (! transform.health())
			continue;
		int n_inlier = get_inliers(transform).size();
		if (update_max(maxinlierscnt, n_inlier))
			best_transform = move(transform);
	}
	inliers = get_inliers(best_transform);
	return fill_inliers_to_matchinfo(inliers, info);
}

Homography TransformEstimation::calc_transform(const vector<int>& matches) const {
	vector<Vec2D> p1, p2;
	for (auto& i : matches) {
		p1.emplace_back(kp1[match.data[i].first]);
		p2.emplace_back(kp2[match.data[i].second]);
	}
	//return ((transform_type == Affine) ? getAffineTransform : getPerspectiveTransform)(p1, p2);

	// normalize the coordinates before DLT, so that points are zero-centered and have sqrt(2) mean distance to origin.
	// suggested by MVG Sec 4.4
	auto normalize = [](vector<Vec2D>& pts) {
		double sizeinv = 1.0 / pts.size();
		Vec2D mean{0, 0};
		// TODO it seems like when camera distortion is severe,
		// mean-subtract leads to more chances of failure
		/*
		 *for (const auto& p : pts) mean += p * sizeinv;
		 *for (auto& p : pts) p -= mean;
		 */

		double sqrsum = 0;
		for (const auto& p : pts) sqrsum += p.sqr() * sizeinv;
		double div_inv = sqrt(2.0 / sqrsum);
		for (auto& p : pts) p *= div_inv;
		return make_pair(mean, div_inv);
	};
	auto param1 = normalize(p1),
			 param2 = normalize(p2);

	// homo from p2 to p1
	Homography h_transform_normalized;
	if (transform_type == Affine) {
		h_transform_normalized = Homography{getAffineTransform(p1, p2)};
	} else if (transform_type == Homo) {
		h_transform_normalized = Homography{getPerspectiveTransform(p1, p2)};
	} else { // Translation
		h_transform_normalized = getTranslationTransform(p1, p2); // p1 and p2 are already normalized.
	}

	Homography t1{{param1.second, 0, -param1.second * param1.first.x,
								 0, param1.second, -param1.second * param1.first.y,
								 0, 0, 1}};
	Homography t2{{param2.second, 0, -param2.second * param2.first.x,
								 0, param2.second, -param2.second * param2.first.y,
								 0, 0, 1}};
	// return transform on non-normalized coordinate
	Homography ret = t1.inverse() * h_transform_normalized * t2;
	return ret;
}

vector<int> TransformEstimation::get_inliers(const Homography& trans) const {
	float INLIER_DIST = sqr(ransac_inlier_thres);
	TotalTimer tm("get_inlier");
	vector<int> ret;
	int n = match.size();

	Matrix transformed = f2_homo_coor.prod(trans.transpose().to_matrix());	// nx3
	REP(i, n) {
		const Vec2D& fcoor = kp1[match.data[i].first];
		double* ptr = transformed.ptr(i);
		double idenom = 1.f / ptr[2];
		double dist = (Vec2D{ptr[0] * idenom, ptr[1] * idenom} - fcoor).sqr();
		if (dist < INLIER_DIST)
			ret.push_back(i);
	}
	return ret;
}

bool TransformEstimation::fill_inliers_to_matchinfo(
		const std::vector<int>& inliers, MatchInfo* info) const {
	TotalTimer tm("fill inliers");
	info->confidence = -(float)inliers.size();		// only for debug
	int min_inliers = (transform_type == Translation) ? 4 : ESTIMATE_MIN_NR_MATCH;
	if (inliers.size() < min_inliers)
		return false;

	// get the number of matched point in the polygon in the first/second image
	auto get_match_cnt = [&](vector<Vec2D>& poly, bool first) {
		if (poly.size() < 3) return 0;
		auto pip = PointInPolygon(poly);
		int cnt = 0;
		for (auto& p : match.data)
			if (pip.in_polygon(first ? kp1[p.first] : kp2[p.second]))
				cnt ++;
		return cnt;
	};
	// get the number of keypoint in the polygon
  // TODO shouldn't count undistinctive keypoints as keypoints.
  // They should get filtered out earlier
	auto get_keypoint_cnt = [&](vector<Vec2D>& poly, bool first) {
		auto pip = PointInPolygon{poly};
		int cnt = 0;
		for (auto& p : first ? kp1 : kp2)
			if (pip.in_polygon(p))
				cnt ++;
		return cnt;
	};

	auto homo = calc_transform(inliers);			// from 2 to 1
	Matrix homoM = homo.to_matrix();
	bool succ = false;
	Homography inv = homo.inverse(&succ);
	if (not succ)	// cannot inverse. ill-formed.
		return false;
  // TODO guess if two images are identical
	auto overlap = overlap_region(shape1, shape2, homoM, inv);
	float r1m = inliers.size() * 1.0f / get_match_cnt(overlap, true);
	if (r1m < INLIER_IN_MATCH_RATIO) return false;
	float r1p = inliers.size() * 1.0f / get_keypoint_cnt(overlap, true);
	if (r1p < 0.01 || r1p > 1) return false;

	Matrix invM = inv.to_matrix();
	overlap = overlap_region(shape2, shape1, invM, homo);
	float r2m = inliers.size() * 1.0f / get_match_cnt(overlap, false);
	if (r2m < INLIER_IN_MATCH_RATIO) return false;
	float r2p = inliers.size() * 1.0f / get_keypoint_cnt(overlap, false);
	if (r2p < 0.01 || r2p > 1) return false;
	print_debug("r1mr1p: %lf,%lf, r2mr2p: %lf,%lf\n", r1m, r1p, r2m, r2p);

	info->confidence = (r1p + r2p) * 0.5;
	if (info->confidence < INLIER_IN_POINTS_RATIO)
		return false;

  double area = polygon_area(overlap);
  double area1 = shape1.w * shape1.h, area2 = shape2.w * shape2.h;
  print_debug("OverlapArea=%.0lf, ImageArea=%.0lf\n", area, max(area1, area2));
  if (area / max(area1, area2) < 0.15)
    return false;

	// fill in result
	info->homo = homo;
	info->match.clear();
	for (auto& idx : inliers)
		info->match.emplace_back(
				kp1[match.data[idx].first],
				kp2[match.data[idx].second]);
	return true;
}

Homography TransformEstimation::getTranslationTransform(const std::vector<Vec2D>& p1, const std::vector<Vec2D>& p2) const {
    if (p1.empty() || p1.size() != p2.size()) {
        // Return identity if input is invalid. RANSAC expects a valid homography.
        // An unhealthy homography might also work if health() check fails.
        return Homography::I(); 
    }

    double total_tx = 0;
    double total_ty = 0;
    for (size_t i = 0; i < p1.size(); ++i) {
        total_tx += (p1[i].x - p2[i].x);
        total_ty += (p1[i].y - p2[i].y);
    }

    double tx = p1.empty() ? 0 : total_tx / p1.size(); // Avoid division by zero if p1 was empty after all (though checked)
    double ty = p1.empty() ? 0 : total_ty / p1.size();

    // Homography is initialized by {h11, h12, h13, h21, h22, h23, h31, h32, h33}
    return Homography({
        1, 0, tx,
        0, 1, ty,
        0, 0, 1
    });
}

}
