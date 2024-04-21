# Data Self Organisation-v (Experimental)

This application demonstrates cellular automata, genetic algorithms and paralllel programming using Sycl and Intel's OneApi toolkit.

This is a work in progress to investigate how a data storage solution may self organise into efficient data structures, for storage and retrieval of data, with the aim of identifying data patterns for arbitrary sentence structures (i.e. sequential data).

The underlying principal is to test if a fundamentally chaotic system can self-organise into a stable data storage and retrieval system.

This technically is contiuning early efforts to develop an alternative "machine learning" algorithm based on cellular automata alone, giving the potential advantages of;

- Integer based calculations only

- Low power consumption during training (comparitively)

- Train once, with data decisions made dynamically during input, assuming there is a fundamental common pattern to how data is stored (this is envisaged to be enabled via a reimagining of the Turing test, given several logic based problems during "training" to resolve, training on the patterns of data, rather than the data itself)

- Not requiring vast amounts of training data to "understand" input

- More transparent operation on how it may reach it's decisions

This version differs from https://github.com/phill-holland/data-self-organisation-iv (it's predecessor) in several ways;

- Implementation of "linked" outputs, a pre-built word list associated with each data cell, to ease with long chains of output words, mitigating input daata having to collide with each required output word

- Word hash values running in the GPU are now stored in triplets, inside of singles (three words at once)

- Various critical bug fixes! (a single program running in a population would yield different output results when that program was run on it's own, related to the boundaries() function and not tracking data not removed from the system successfully)

- Additional debugging functions to help diagnosis bugs

The existing trained "program" now provided in the directory and filename data/saved/run.txt, will now sing part of the daisy, daisy song with you! (demonstrating it's ability to store and retrieve output sentences according to complex input)

# Method

Data is given a 3D dimension cube in order to move around in, one grid cell at a time.  You can insert sentences into the system, using varyingly different randomly generated patterns, and pre-load the 3D cube with static data before the main loop starts.

Data is configured to behave in different ways when they collide with one another, potentially changing it's path of direction.

Output is determined when one data cell collides with another static data cell.

For this test program, we are using the song Daisy Daisy as a small data input test sample.

```
daisy daisy give me your answer do .
I'm half crazy for the love of you .
it won't be a stylish marriage .
I can't afford a carriage .
but you'll look sweet upon the seat .
of a bicycle built for two .
```

These input words are tokenised into integers and stored within an internal dictionary.

# Demonstrates

- Genetic Algorithms (semi pareto dominate calcuation)

- Cellular Automata

- Sycl/OneApi parallel GPU code 

- Triple buffer for general population, to maximise concurrency and reduce data locking

- Custom method for scoring and comparing sentences against each other, more concise that alternate algorithms, like BOW (Bag Of Words)

# Scoring

This system implements a unique method for comparing sentences, based on their relative position to each other, rather than just by their fixed
index within a goal sentence.

For instance if we take the goal sentence of;

```
daisy daisy give me your answer do
```

and score it against the example output of;

```
your answer do bicycle carriage
```

The word chain of 'your answer do' is a partial correct answer, but however, it's positioning within the desired sentence is wrong.  The system ranks more highly the fact that 'your' is in front of 'answer' and 'do' follows it.

This method ensures it encourages partial answers, but with differing word positioning errors, for each subsequent iteration and generation, it should gradually correct those positioning errors.  This method seems to be better suited to sentence matching given this unique application of genetic algorithms.

# Todo

- <strike>Implement different collision methods, by "age" of data in system</strike> postponed in favour of alternative system

- <strike>Limit inputted word age in the system by some mechanism, to enable length of output to be limited in some way (words must die to stop infinite output!)</strike> postponed in favour of alternative system (see below)

- It's clear in experiments that inputted data into the system must "log" those other bits of data it collides with, in order to preserve the sequence order of the words of an inputted sentence (as in the newly implemented "word" links system), the next iteration of the system will be to allow data dynamically inputted into the system in the same epoch, and then immediately queried.

- Data words inputted into the system movement should eventually slow down, and stop, become part of the system, that when collided with, will then output it's associated data.

- <strike>Extend non-commutative collision calculate to include impacts with stationary data objects stored in the cache class (parallel/program.cpp line 524)</strike>, implemented

- <strike>Retest long sentence chains as input as well as output</strike> implemented

- <strike>Cache, add stationary data cells, that do not output data when collided with (a pseudowall)</strike> implemented & fix, via the links system

- Collision chain is currently limited to a single "rebound" movement before returning to default movement behaviour, extend
collision movements to be > 1

- <strike>Fix collision detection of native CPU code (native code has somewhat been ignored!)</strike> will not fix

- <strike>Experiment with different scoring methods, one that treats the system less like a black box, and uses internal information
to score output</strike> data-self-organisation-iii, abandoned

- Update cmake files to use latest 2024 version of Intel OneApi

# Problems

- System now needs to be extended to allow dynamic input of data, and then queried on that input, with further input, leaving some interesting programs, how to tell the system the difference between inputted data, and a question posed to the system?

- <strike>It get's slower and longer to find solutions the more "sentences" and epochs you run and configure it to look for</strike> mitigated and improved with additions

- <strike>It get's slower when you're looking for long output sentences (more than 5 words)</strike> mitigated and improved with addition of links system

- <strike>It only needs one input word, to find a longer output sentence (encouraging input words to interact with each other whilst
in the system, for example by encouraging more word collisions makes the genetic algorithm less performant)</strike> next version which promotes collisions will mitigate this

- May need running several times with the same parameters to find the correct answer (a common drawback of genetic algorithms)

# Potential Alternative Applications

It's conceivable that you can use this method for the encryption of messages, to generate a decryption key, the method is as followed;

- Generate a random sentence, with those words as the input dictionary of the system

- Define the message to be encrpyted as the desired output of the system

- Run the system until the output matches the random sentence generated in step one

This has several advantages, the input and output messages have no relationship with one another beyond this system that generated the "key", no information about the encrypted method is stored within the output.

The current weakness of the system is that you could potentially put any input words into the system to get an output message, this can be mitigated by encouraging more "input word" interactions with one another within the system, this can be handled by increasing this setting value, shown below;

```
parameters.scores.max_collisions = 0;
```

Increasing this number should encourage a more chaotic unpredictable behaviour of the system, the higher the number, the better it's ability to encrypt unique messages.

There are still drawbacks to this usage process though, the key in it's current form is not reusable, for each message you encrypt, you need to generate a new key, which then causes big flaws in the system, such as needing to exchange a new key each time you wish to encrypt a message, rendering the idea mostly a toy rather than having any current practical benefit.

There's an outside chance given more work this drawback can be overcome, but it may currently be technically improbable (?)

# Building & Running (see requirements below)

- Project comes with VSCode pre-configured launch and build tasks, however to manually build;

```
source /opt/intel/oneapi/setvars.sh --include-intel-llvm --force
mkdir build
cd build
cmake -DCMAKE_CXX_FLAGS="-O0 -g" -DBUILD_FOR_CUDA=true ../
cmake --build .
```

Assuming Intel OneApi is pre-installed and you have a newish Nvidia GPU (it will probably compile and run on GTX 10xx series and above)

Running the tests can be accomplished by;

```
cd build
ctest
```

The build process should pop an executable file in the build directory to run the application, it is advised to
run the system from within VSCode, to tweak the input parameters!

# Requirements

- Originally written on a Linux based operating system.

- This project is written in C++ and Sycl and requires that the Intel OneApi SDK is installed. (currently supporting version 2023, and 2024 with minor modifications)

- This project will compile using Cmake and requires support for version 3.20 or above.

- Nvidia's Cuda must also be installed, in order for you to target Nvidia GPUs.


