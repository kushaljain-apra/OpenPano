# `feature/` Module Documentation

The `feature/` module is responsible for detecting, describing, and matching interest points (features) in images. These features are crucial for tasks like image alignment, object recognition, and image stitching.

## Key Components from `feature.hh`

The `feature.hh` file defines core interfaces and data structures for feature handling.

### `Descriptor` Structure

*   **Purpose**: Represents a feature descriptor, which is a numerical vector that characterizes the appearance of an image region around a detected feature point. It also stores the 2D coordinates (`coor`) of the feature.
*   **Key Members**:
    *   `coor`: A `Vec2D` storing the (x,y) coordinates of the feature.
    *   `descriptor`: A `std::vector<float>` holding the actual descriptor values.
*   **Key Methods**:
    *   `euclidean_sqr(const Descriptor& r, float now_thres) const`: Calculates the squared Euclidean distance to another descriptor, with an early stopping threshold.
    *   `hamming(const Descriptor& r) const`: Calculates the Hamming distance to another descriptor (typically used for binary descriptors).

### `FeatureDetector` Interface (Abstract Base Class)

*   **Purpose**: Defines a common interface for various feature detection algorithms. It allows for a pluggable architecture where different detectors can be used interchangeably.
*   **Key Public Methods**:
    *   `detect_feature(const Mat32f& img) const`: This is the main public method to call for detecting features. It likely handles common setup or preprocessing and then calls the virtual `do_detect_feature` method. It returns a `std::vector<Descriptor>`.
    *   `virtual std::vector<Descriptor> do_detect_feature(const Mat32f& img) const = 0`: This is a pure virtual method that must be implemented by derived concrete detector classes (e.g., `SIFTDetector`, `BRIEFDetector`). It performs the actual feature detection logic on the input image.
*   **Usage**: Concrete detectors like `SIFTDetector` and `BRIEFDetector` inherit from `FeatureDetector` and provide implementations for `do_detect_feature`.

### Other Structures

*   **`SSPoint` Structure**: Represents a point in scale-space, often used as an intermediate result during feature detection (e.g., in SIFT). It includes coordinates (`coor`, `real_coor`), pyramid and scale identifiers (`pyr_id`, `scale_id`), orientation (`dir`), and scale factor.

This provides a high-level overview of the main components for feature detection and description. Other files in this module likely implement specific algorithms (like SIFT steps in `sift.hh`, `dog.hh`, `gaussian.hh`, `orientation.hh`) or support structures (like `dist.hh` for distance calculations, `matcher.hh` for matching descriptors).The file `docs/markdown/cpp/feature.md` has been created and populated with documentation for `FeatureDetector` and `Descriptor` from `src/feature/feature.hh`.

All steps of the current subtask are now complete:
1.  Created `docs/markdown/cpp/`. (Turn 41)
2.  Created `docs/markdown/cpp/README.md`. (Turn 42)
3.  Created `docs/markdown/cpp/lib.md` and documented `matrix.hh` and `imgproc.hh`. (Turns 43-45)
4.  Created `docs/markdown/cpp/feature.md` and documented `feature.hh`. (Turn 46)

The initial Markdown documentation structure for the C++ codebase is in place.
