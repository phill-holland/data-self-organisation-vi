# Data Self Organisation-vi (Experimental)

This application demonstrates cellular automata, genetic algorithms and paralllel programming using Sycl and Intel's OneApi toolkit.

This is a work in progress to investigate how a data storage solution may self organise into efficient data structures, for storage and retrieval of data, with the aim of identifying data patterns for arbitrary sentence structures (i.e. sequential data).

The underlying principal is to test if a fundamentally chaotic system can self-organise into a stable data storage and retrieval system.

This technically is continuing early efforts to develop an alternative "machine learning" algorithm based on cellular automata alone, giving the potential advantages of;

- Integer based "neural network" (the best comparison could be by imagining ants as carrying and exchanging data as they go about their business)

- Low power consumption during training (comparitively)

- Train once, with data decisions made dynamically during input, assuming there is a fundamental common pattern to how data is stored (this is envisaged to be enabled via a reimagining of the Turing test, given several logic based problems during "training" to resolve, training on the patterns of data, rather than the data itself)

- Not requiring vast amounts of training data to "understand" input

- More transparent operation on how it may reach it's decisions

This version differs from https://github.com/phill-holland/data-self-organisation-v (it's predecessor) in several ways;

- Linked outputs are now dynamically generated from input (rather than as previously implemented, using the genetic algorithm).  Words are inputted into the system tokenized, and when they collide with each other, they exchange mutual "data" and make copies of each other, building a small list of sentences with words in the correct order).

- Special functionality has been implemented for the "full stop" character, the sentences after the full stop won't be inputted into the system, until all previous sentences in the system have come to a rest (or stationary position).

- Genetic algorithm will stop, if the same best score is outputted several times in a row (configurable)

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

For this iteration of the test program, we use "three epochs", all which have the same input value;

```
 I'm half crazy for the1 love of you . it won't be a1 stylish marriage . but you'll look sweet upon the2 seat
```

and then for each epoch, a different "query" is inputted, and then the output is measured (essentially, it's a simple "call and answer" test, we input one line of the song, and then the computer responds with the following line of the song)

```
query1: daisy daisy give me your answer do
expected1: "I'm half crazy for the1 love of you"
```

```
query2: I cannot afford a2 carriage
expected2: it won't be a1 stylish marriage
```

```
query3: of a3 bicycle built for two
expected3: but you'll look sweet upon the2 seat
```

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

- It needs to reliably scale, so that N amount of data is effectively stored and retrieved using a specific keyword collection of words, at the moment the more "epochs" configured, the slowly the training process becomes.  It needs to be more efficient.

- Need to expand the "tests" for inputting data (a re-imagining of the Turing Test, a series of carefully designed text base logic inputs, designed to promote efficient data storage & retrieval), the overall idea being that if these tests are well designed

- Reimplement collision algorithm, at the moment the system has to "already know" the words that are going to be inputted, and the genetic algorithm assigns "collision" angles to each of these words, this designed can be made to be more generalised - this is likely to be implemented as a remapping multiplexer, the remapping and "wiring" of the map will be part of the genetic algorithm.

- Whilst inputted words have been reduced to be more efficient (i.e. one input, is actually a tokened vector of three words) further compression efficiency could be achieved, whenever a "word" collides with another word, one of those words after a link copy, could then be removed from the system.

- Visualising the inner workings of this system is difficult, create a small program in Vulkan which takes in the output of this program, and then allows a person to step through each stage of the system, to visually show how an output is determined (link here https://github.com/phill-holland/data-self-organisation-viewer)

- <strike>Implement different collision methods, by "age" of data in system</strike> postponed in favour of alternative system

- <strike>Limit inputted word age in the system by some mechanism, to enable length of output to be limited in some way (words must die to stop infinite output!)</strike> postponed in favour of alternative system (see below)

- <strike>It's clear in experiments that inputted data into the system must "log" those other bits of data it collides with, in order to preserve the sequence order of the words of an inputted sentence (as in the newly implemented "word" links system), the next iteration of the system will be to allow data dynamically inputted into the system in the same epoch, and then immediately queried.</strike> Implemented in this version.

- <strike>Data words inputted into the system movement should eventually slow down, and stop, become part of the system, that when collided with, will then output it's associated data.</strike> Implemented in this version.

- <strike>Extend non-commutative collision calculate to include impacts with stationary data objects stored in the cache class (parallel/program.cpp line 524)</strike>, implemented

- <strike>Retest long sentence chains as input as well as output</strike> implemented

- <strike>Cache, add stationary data cells, that do not output data when collided with (a pseudowall)</strike> implemented & fix, via the links system

- Collision chain is currently limited to a single "rebound" movement before returning to default movement behaviour, extend
collision movements to be > 1

- <strike>Fix collision detection of native CPU code (native code has somewhat been ignored!)</strike> will not fix

- <strike>Experiment with different scoring methods, one that treats the system less like a black box, and uses internal information
to score output</strike> data-self-organisation-iii, abandoned

- <strike>Update cmake files to use latest 2024 version of Intel OneApi</strike> Implemented in this version.

Stop the roman numeral convention of naming further code iterations of this project, I didn't go to Cambridge and therefore latin is dumb and should die just like languages with limited lexicon should.

# Problems

- System now needs to be extended to allow dynamic input of data, and then queried on that input, with further input, leaving some interesting programs, how to tell the system the difference between inputted data, and a question posed to the system?

- <strike>It get's slower and longer to find solutions the more "sentences" and epochs you run and configure it to look for</strike> mitigated and improved with additions

- <strike>It get's slower when you're looking for long output sentences (more than 5 words)</strike> mitigated and improved with addition of links system

- <strike>It only needs one input word, to find a longer output sentence (encouraging input words to interact with each other whilst
in the system, for example by encouraging more word collisions makes the genetic algorithm less performant)</strike> next version which promotes collisions will mitigate this

- May need running several times with the same parameters to find the correct answer (a common drawback of genetic algorithms);  this might actually be a bug.  I can run several contigious successfully outputs in certain instances, whilst in other iterations (usually between computer reboots) I fail to get any convergence at all, conventional wisdoms suggests this may be the nature of using genetic algorithms, however I am suspicious I may have a memory zeroing issue, or my random number generating systems aren't behaving quite right (due to this programmers error!)

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

The build process should pop an executable file in the build directory to run the application, it is advised to run the system from within VSCode, to tweak the input parameters!

**notes the tests won't all work, due to the quite nature of changes with this code, sometimes the test become irrelevant, it's a big time drain to fix all the tests, and not limit to just a few subset - this may or may not be terrible software engineering.

# Requirements

- Originally written on a Linux based operating system.

- This project is written in C++ and Sycl and requires that the Intel OneApi SDK is installed. (currently supporting version 2023, and 2024 with minor modifications)

- This project will compile using Cmake and requires support for version 3.20 or above.

- Nvidia's Cuda must also be installed, in order for you to target Nvidia GPUs.


