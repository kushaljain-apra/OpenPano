# `stitch/` Module Documentation

The `stitch/` module is dedicated to the process of image stitching, which involves combining multiple images with overlapping fields of view to produce a panoramic or high-resolution image. This module includes components for camera parameter estimation, image warping, blending, and the overall stitching pipeline control.

## Key Components

### `StitcherBase` Class (`stitcherbase.hh`)

*   **Purpose**: The `StitcherBase` class serves as an abstract base class for various image stitching algorithms. It provides a common framework for managing a collection of input images (`ImageRef` objects) and their corresponding detected features (`Descriptor` objects). It handles the initialization of a feature detector (e.g., SIFTDetector) and provides a mechanism to calculate and store features for all input images.
*   **Key Protected Members**:
    *   `imgs`: A `std::vector<ImageRef>` storing references to the input images to be stitched.
    *   `feats`: A `std::vector<std::vector<Descriptor>>` to store the detected features for each image. Coordinates are typically in a system relative to the image center (e.g., `[-w/2, w/2]`).
    *   `keypoints`: A `std::vector<std::vector<Vec2D>>` storing the coordinates of keypoints, likely mirroring the coordinates in `feats`.
    *   `feature_det`: A `std::unique_ptr<FeatureDetector>` holding an instance of a feature detector.
*   **Key Protected Methods**:
    *   `calc_feature()`: A method to calculate features for all images stored in `imgs` using the `feature_det`.
    *   `free_feature()`: A method to release the memory occupied by the stored features.
*   **Key Public Methods**:
    *   **Constructor**: Takes a universal reference to a collection of image representations (e.g., file paths or image data wrappers) and initializes the `imgs` vector. It also typically initializes the `feature_det`.
    *   `virtual Mat32f build() = 0`: A pure virtual method that must be implemented by derived classes. This method is responsible for executing the specific stitching pipeline (including steps like feature matching, homography estimation, warping, and blending) and returning the final stitched image as a `Mat32f`.
    *   **Destructor**: A virtual default destructor to ensure proper cleanup of derived classes.
*   **Usage**: Concrete stitcher implementations (e.g., `CylindricalStitcher`, `SphericalStitcher`) would inherit from `StitcherBase`, implement the `build()` method, and potentially override other behaviors or add specific parameters for their stitching strategy.

### `Camera` Class (`camera.hh`)

*   **Purpose**: Represents the intrinsic and extrinsic parameters of a camera. It stores information like focal length, principal point, aspect ratio, and rotation, which are essential for 3D transformations and image warping in the stitching process.
*   **Key Public Members (Parameters)**:
    *   `focal`: A `double` representing the focal length of the camera. Defaults to 1.
    *   `aspect`: A `double` for the aspect ratio of the pixels. Defaults to 1.
    *   `ppx`: A `double` for the x-coordinate of the principal point. Defaults to 0.
    *   `ppy`: A `double` for the y-coordinate of the principal point. Defaults to 0.
    *   `R`: A `Homography` object (likely a 3x3 matrix) representing the rotation of the camera.
*   **Key Public Methods**:
    *   `K() const`: Returns the intrinsic matrix (`Homography`) calculated from `focal`, `aspect`, `ppx`, and `ppy`.
    *   `Kinv() const`: Returns the inverse of the intrinsic matrix.
    *   `Rinv() const`: Returns the inverse of the rotation matrix (i.e., `R.transpose()`).
*   **Key Static Methods**:
    *   `estimate_focal(const std::vector<std::vector<MatchInfo>>& matches)`: Estimates the focal length based on feature matches between images.
    *   `rotation_to_angle(const Homography& r, double& rx, double& ry, double& rz)`: Converts a rotation matrix to Euler angles (rx, ry, rz).
    *   `angle_to_rotation(double rx, double ry, double rz, Homography& r)`: Converts Euler angles to a rotation matrix.
    *   `straighten(std::vector<Camera>&)`: A static method that likely adjusts the rotation of a set of cameras to make them "straight" or aligned, possibly to a common plane or horizon.
*   **Operators**: Includes an output stream operator `<<` for printing camera parameters.

### Blending Classes (`blender.hh`)

The `blender.hh` file defines classes for blending multiple warped images together to create a seamless panorama.

*   **`BlenderBase` Class**:
    *   **Purpose**: An abstract base class defining the interface for image blending operations. It manages a list of images to be added to the panorama, along with their spatial ranges and coordinate mapping functions.
    *   **`Range` Struct**: A nested struct within `BlenderBase` to define a rectangular region (min and max coordinates).
    *   **`ImageToAdd` Struct**: A nested struct to hold information about an image that needs to be blended, including its `Range` on the target canvas, a reference to the `ImageRef`, and a `coor_func` (a function object, likely `std::function<Vec2D(Coor)>`) that maps coordinates from the target canvas back to the source image.
    *   **Key Public Methods**:
        *   `virtual ~BlenderBase()`: Virtual destructor.
        *   `virtual void add_image(...) = 0`: Pure virtual method to add an image to the blender. Takes the image's bounding box (`upper_left`, `bottom_right`) on the target canvas, the `ImageRef` itself, and the coordinate mapping function.
        *   `virtual Mat32f run() = 0`: Pure virtual method that executes the blending algorithm and returns the final blended image as a `Mat32f`.

*   **`LinearBlender` Class**:
    *   **Purpose**: A concrete implementation of `BlenderBase` that likely performs linear blending (e.g., alpha blending or weighted averaging) where pixel values in overlapping regions are averaged.
    *   **Inheritance**: Inherits from `BlenderBase`.
    *   **Key Public Methods**:
        *   `void add_image(...) override`: Implements the base class method to add an image to a list of images to be blended. It likely determines the overall `target_size` of the final panorama based on the ranges of images added.
        *   `Mat32f run() override`: Implements the blending logic. It iterates over the target canvas pixels, and for each pixel, it identifies overlapping source images using the `map_coor` function from `ImageToAdd`. It then interpolates and averages the colors from the source images to produce the final pixel value.
        *   `void debug_run(int w, int h)`: A helper method likely used for debugging, possibly to render each component image or intermediate blending stages.

Other important components in this module likely include:
- Camera parameter estimation (`camera_estimator.hh`, `homography.hh`, `transform_estimate.hh`)
- Image warping and projection (`warp.hh`, `projection.hh`)
- Specific stitcher implementations (`cylstitcher.hh`, `stitcher.hh`)
- Image blending techniques (`multiband.hh`)
- Data structures for matches and image references (`match_info.hh`, `imageref.hh`)
