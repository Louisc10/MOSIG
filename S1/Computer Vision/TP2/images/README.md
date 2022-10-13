We Have 3 Files

# filtering_3x3
This file is for creating the image with Gaussian Filter with filter 3x3.
## How to compile
> make filtering_3x3

## How to use
>./filtering_3x3 [filename]>[file_output] [times_of_repetition]

you need to pass some arguments in here.
**filename** is the picture that you want to modified
**file_output** is the name of output picture
**times_of_repetition** is the total times of filter using gaussian blur (3x3)

# filtering_5x5
This file is for creating the image with Gaussian Filter with filter 5x5.
## How to compile
> make filtering_5x5

## How to use
>./filtering_5x5 [filename]>[file_output] [times_of_repetition]

you need to pass some arguments in here.
**filename** is the picture that you want to modified
**file_output** is the name of output picture
**times_of_repetition** is the total times of filter using gaussian blur (5x5)

# filtering
This file is for creating the image with Gaussian Filter where you can specify the size of the filter manually.
## How to compile
> make filtering

## How to use
>./filtering [filename]>[file_output] [size_of_the_blur] [times_of_repetition]

you need to pass some arguments in here.
**filename** is the picture that you want to modified
**file_output** is the name of output picture
**size_of_the_blur** is the size of filter (size_of_the_blur x size_of_the_blur) expected in odd number and greater than 0
**times_of_repetition** is the total times of filter using median

# filter_median
This file is for creating the image with Median Filter where you can specify the size of the filter manually.
## How to compile
> make filter_median

## How to use
>./filter_median [filename]>[file_output] [size_of_the_blur] [times_of_repetition] 

you need to pass some arguments in here.
**filename** is the picture that you want to modified
**file_output** is the name of output picture
**size_of_the_blur** is the size of filter (size_of_the_blur x size_of_the_blur) expected in odd number and greater than 0
**times_of_repetition** is the total times of filter using median

# histogram
This file is for creating the image with Gaussian Filter where you can specify the size of the filter manually and also add Histogram Modification at the result
of the image.
## How to compile
> make histogram

## How to use
>./histogram [filename]>[file_output] [size_of_the_blur] [times_of_repetition] [histogram_mode]

you need to pass some arguments in here.
**filename** is the picture that you want to modified
**file_output** is the name of output picture
**size_of_the_blur** is the size of filter (size_of_the_blur x size_of_the_blur) expected in odd number and greater than 0
**times_of_repetition** is the total times of filter using Gaussian Filter
**histogram_mode** is the mode of histogram transformation expected value "s" or "e"