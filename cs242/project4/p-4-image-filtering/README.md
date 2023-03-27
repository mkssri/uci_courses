# Project 4: Image Filtering and Edge Detection


## Implementations
For our project we implemented image filtering and edge detection in six ways. 

- An in-place 2x2 single threaded convolutional, that served to test our convolutional performance and expand functionality.
- An in-place 3x3 single threaded convolutional, that demonstrated the process and is used as a baseline.
- An out-of-place 3x3 single threaded convolutional, to test performance impact of out-of-place transactions and as a baseline.
- An out-of-place 3x3 multi-threaded convolutional, which multithreads by dividing the image into its RGB components.
- An out-of-place 3x3 multi-threaded convolutional, which multithreads by dividing the image into lines.
- An in-place 3x3 multi-threaded convolutional, which multithreads on the RGB components.



### In-place 2x2
#### Theoretical concept

#### Code organization

### In-place 3x3
#### Theoretical concept

#### Code organization

### Out-of-place 3x3
#### Theoretical concept

#### Code organization

### Out-of-place 3x3 Multithreaded Color
#### Theoretical concept

#### Code organization

### Out-of-place 3x3 Multithreaded Lines
#### Theoretical concept

#### Code organization

### In-place 3x3 Multithreaded Color
#### Theoretical concept
.ppm's are composed of 3 parallels matrices of R,G and B. This lends itself to intuitive multithreading by simply handling each one of the color spaces seperately. By treating each as a independant matrix, we can implement multithreading as a independent thread per color without worrying about race conditions and data loss.

#### Code organization
The three color matrices are passed into the function, where each is assigned its own thread. Each thread is responsible for running in-place convolutions along the entire matrix before joining when it concludes. As in-place single matrix convolutions was a solved issue from prior, we could implement an altered version of the 3x3 in-place single algorithm. 

## How to Run Code
1. Download Code from repo
2. In terminal run "make clean all"
3. In terminal run "./main"

## Metrics and tests
- **How** are you defining "Good use of multiple cores".
To us, a good use of multiple cores involves efficently constructing our algorithms in a manner that can benefit from parallel operations. Indeed, it is about being able to leverage the full performance of a multithreaded system to accelerate a traditionally single-threaded workload. It also means seeing an increase in performance relatively related to how many cores we can leverage.

- **What** are the experiments/benchmarks that you ran in order to measure the metrics described above.
We benchmarked our algorithms by timing their absolute runtime on identical images. We used our in-place and out-of-place single threaded algorithms as our benchmarks and calculated the approximate speed up for our various iterations.

- What environment did you use to run your experiments.
We used the CS Lab machines to run the experiments. We ran on a Xeon X5680 @ 3.33GHz with 6 cores.

- What tests did you run to verify that the actual multiplication is correct.
To verify our tests were correct, we implemented a mat_equal function which compares two matrices for any differences in values. We then ran this on all our results to ensure all our tests returned a value consistent with our expected returns.


## Findings and Interpretations
- What were your findings.
![](filtering_images_v1.png)
- How can they be interpreted.
- What is the take-away of this project.
- What did you learn.
- What can be done in the future.
