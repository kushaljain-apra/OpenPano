# `lib/` Module Documentation

The `lib/` module provides core library functionalities that are fundamental to the computer vision tasks performed in this project. It includes data structures for matrices, image processing routines, configuration handling, and various utility functions.

## Key Components

### Matrix Operations (`matrix.hh`)

The `matrix.hh` file defines a `Matrix` class for performing dense matrix operations.

*   **`Matrix` Class**:
    *   **Purpose**: Represents a 2D matrix of `double` values and provides a comprehensive suite of linear algebra operations. It appears to be a wrapper or extension of a more generic `Mat<T>` class.
    *   **Constructors**: Allows creation of empty matrices, matrices of specified dimensions, and copy construction from another `Mat<double>`.
    *   **Key Public Methods**:
        *   `inverse(Matrix& ret) const`: Computes the inverse of the matrix.
        *   `pseudo_inverse() const`: Computes the Moore-Penrose pseudo-inverse.
        *   `transpose() const`: Returns the transpose of the matrix.
        *   `prod(const Matrix& r) const` (and `operator*`): Performs matrix multiplication.
        *   `elem_prod(const Matrix& r) const`: Performs element-wise matrix multiplication.
        *   `operator-`, `operator+`: Perform matrix subtraction and addition.
        *   `SVD(Matrix& u, Matrix& s, Matrix& v) const`: Performs Singular Value Decomposition.
        *   `normrot()`: Likely normalizes or rotates the matrix.
        *   `sqrsum() const`: Computes the sum of squares of all elements.
        *   `col(int i) const`: Extracts a specific column.
        *   `zero()`: Sets all elements to zero.
        *   `static Matrix I(int)`: Creates an identity matrix of a given size.
    *   **Operators**: Overloads common arithmetic operators like `*`, `+`, `-`. Also includes an output stream operator `<<` for printing.

### Image Processing (`imgproc.hh`)

The `imgproc.hh` file provides a collection of free functions for various image processing tasks.

*   **Image I/O**:
    *   `read_img(const char* fname)`: Reads an image into a `Mat32f` (likely a matrix of 32-bit floats, possibly representing RGB channels).
    *   `read_img_uc(const char* fname)`: Reads an image into a `Matuc` (likely a matrix of unsigned chars).
    *   `write_rgb(const char* fname, const Mat32f& mat)`: Writes a `Mat32f` image to a file.
*   **Image Manipulation**:
    *   `hconcat(const std::list<Mat32f>& mats)`: Horizontally concatenates a list of images.
    *   `vconcat(const std::list<Mat32f>& mats)`: Vertically concatenates a list of images.
    *   `interpolate(const Mat32f& mat, float r, float c)` / `interpolate(const Matuc& mat, float r, float c)`: Interpolates color at a given floating-point coordinate in an image.
    *   `crop(const Mat32f& mat)`: Crops an image (details of cropping logic would require inspecting the .cc file or further documentation).
    *   `rgb2grey(const Mat32f& mat)`: Converts an RGB image to grayscale.
    *   `fill(Mat32f& mat, const Color& c)` / `fill(Mat32f& mat, float c)`: Fills an image with a specified color or float value.
    *   `resize(const Mat<T>& src, Mat<T>& dst)`: Resizes an image.
    *   `cvt_f2uc(const Mat32f& mat)`: Converts a `Mat32f` to `Matuc`.
*   **Geometric Transforms**:
    *   `getPerspectiveTransform(const std::vector<Vec2D>& p1, const std::vector<Vec2D>& p2)`: Calculates the perspective transformation matrix between two sets of points.
    *   `getAffineTransform(const std::vector<Vec2D>& p1, const std::vector<Vec2D>& p2)`: Calculates the affine transformation matrix between two sets of points.

### Other Utilities

The `lib/` module also contains utilities for:
- Color manipulation (`color.hh`)
- Configuration loading (`config.hh`)
- Debugging (`debugutils.hh`)
- Geometric calculations (`geometry.hh`)
- Image I/O (`imgio.cc` - likely paired with a header)
- KD-Tree data structures (`kdtree.hh`)
- Planar drawing (`planedrawer.hh`)
- Polygon manipulation (`polygon.hh`)
- Timing utilities (`timer.hh`)
- General utilities (`utils.hh`)

These will be documented in more detail later.
