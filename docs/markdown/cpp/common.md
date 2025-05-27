# `common/` Module Documentation

The `common/` module appears to store widely used, miscellaneous utilities and definitions that don't fit into the more specialized modules like `lib/`, `feature/`, or `stitch/`.

## Key Components from `common.hh`

The `common.hh` file provides very basic, project-wide definitions.

*   **MSVC Compatibility**:
    *   `#ifdef MSVC`
    *   `#define not !`
    *   **Purpose**: This preprocessor directive defines the keyword `not` as an alias for the `!` operator when compiling with the Microsoft Visual C++ compiler (MSVC). This is likely to ensure compatibility or maintain a certain coding style, as `not` is an alternative token for `!` in C++.

Other common utilities, if any, would be documented here as they are identified. Based on the current content of `common.hh`, its role is primarily for low-level, compiler-specific compatibility macros.
