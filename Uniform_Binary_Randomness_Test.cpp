/*
    Uniform Binary Randomness Test

    Author: Jason Tong

    Description: 
    This program assesses the randomness of a uniform binary random number generator (RNG) by 
    analyzing the frequencies of groupings of 0s and 1s, with group sizes between 1 and g, in a 
    generated random binary sequence. The program calculates the standard deviation of the 
    occurrences and probabilities of these permutations to evaluate the uniformity of the RNG. 
    With perfect uniformity, the standard deviation of the frequencies of permutations should be 
    close to 0, with the probabilities (or frequencies) of each permutation occurring being 
    approximately equal.

    With true uniform randomness, the frequencies of occurence for 0s and 1s are the same, and so 
    too are the size-2 permutations (00, 01, 10, 11, each with a probability of 0.25), and size-3 
    permutations (000, 001, 010, 011, 100, 101, 110, 111), etc. for all grouping sizes. This can 
    be considered a defining characteristic of true uniform randomness.

    To avoid sampling bias, the user should pick a random binary sequence length that is at least 
    10 times the size of the permutation size for the largest groupings considered, g, i.e., at 
    least 10 * 2^g. 
*/

#include <iostream>
#include <limits>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "Uniform_Binary_Randomness_Test.h"
using namespace std;

int main(){
    int g = 0;
    cout << "This program will test the uniformity of a binary random number generator by comparing the frequencies of g-sized permutations of 0s and 1s.\n"
    << "To produce an accurate and comprehensive result, a large grouping size is recommended, e.g., g=10.\n"
    << "\nEnter the grouping size, g, to consider for the test: ";
    while(!(cin >> g && g > 0)){
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "To continue with the test, please enter a positive integer for the grouping size.\n"
        << "Enter the grouping size, g, to consider for the test: ";
    }

    int length = 0;
    cout << "\nFor measuring the randomness of an RNG, a binary sequence of random numbers will be generated and analyzed.\n"
    << "A long sequence of binary numbers is recommended for a more accurate result, e.g., length=10000.\n"
    << "\nEnter the length of the binary random number sequence to generate: ";
    while(!(cin >> length && length > 0)){
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "To continue with the test, please enter a positive integer for the binary sequence length.\n"
        << "Enter the length of the binary random number sequence to generate: ";
    }

    // Lookup table for the binary permutations of 0s and 1s used when g <= 3
    const char* lookupTable[14] = {"0", "1", "00", "01", "10", "11", "000", "001", "010", "011", "100", "101", "110", "111"};

    // Array allocations
    int* randomArray = new int[length];
    populateRandomBinary(randomArray, length);
    int* occurrences = new int[0b1 << g];

    double standardDeviation = 0; // Standard deviation of the occurrences of the permutations
    double standardDeviationProb = 0; // Standard deviation of the probabilities of the permutations occurring
    int PermutationsCount = 0;
    
    // Considering each grouping size from 1 to g, not considering grouping sizes larger than the random array length
    for (int i = 1; i <= g && i <= length; i++) { 
        if(i < 4)
            cout << "\nFor g=" << i << ": Sampled frequency of permutations and standard deviation\n";
        else
            cout << "\nFor g=" << i << ": Standard deviation of the frequencies of permutations\n";
        
        // Populates the frequencies of the permutations of size i occuring in the random binary array
        findPermutationFrequencies(occurrences, randomArray, length, i);

        // Analyze the frequencies of each permutation of size i
        for (int j = 0; j < (PermutationsCount = 0b1 << i); j++) {  
            standardDeviation += pow(occurrences[j] - (length/i)/(float)PermutationsCount, 2);
            standardDeviationProb += pow(occurrences[j]/(float)(length/i) - 1/(float)(PermutationsCount), 2);
            if (i < 4){ // For grouping sizes 1 to 3, print the estimated probabilities of each permutation occurring from the RNG
                cout << "- Frequency of permutation " << lookupTable[PermutationsCount-2 + j] << ": " << occurrences[j]/(float)(length/i) << endl;
            }
        }
        // For samples, the standard deviation is SD = sqrt(sum[(x - mean)^2]/(n-1)), where n is the number of samples, x is a sample, and mean is the average of the samples.
        standardDeviation = sqrt(standardDeviation/(PermutationsCount - 1));
        standardDeviationProb = sqrt(standardDeviationProb/(PermutationsCount - 1));
        cout << "Standard deviation of occurrences: " << standardDeviation << endl;
        cout << "Standard deviation of probabilities: " << standardDeviationProb << endl;
        standardDeviation = 0;
        standardDeviationProb = 0;
    }

    cout << "\nWith normal distribution, the standard deviation of the frequencies of permutations represents the range in sample values where roughly 70\% of the samples lie within.\n";

    // Deallocate memory from the arrays initialized
    delete[] randomArray;
    delete[] occurrences;
}

/*  Populates a given array with random binary numbers (0 or 1) using 
    the built-in C++ library rand() function.

    array: The array to populate with random binary numbers.
    length: The length of array.
*/
void populateRandomBinary(int array[], int length) {
    srand(time(nullptr));
    for (int i = 0; i < length; ++i) {
        array[i] = rand() % 2; // Generate random binary number, 0 or 1
        // Modulo bias is minimal with modulo 2 for RAND_MAX >= 2^15 - 1
    }
}

/*
    Finds the number of occurrences of the permutation of size g in the given array of 
    random binary numbers. Size-g permutations are considered for non-overlapping sequences.

    occurrences: The array to store the frequencies of each permutation.
    array: The array containing the binary numbers generated by the RNG.
    length: The length of the binary numbers array.
    g: The size of the permutation to consider.
*/
void findPermutationFrequencies(int occurrences[], const int array[], int length, int g) {
    int permutationCount = 0b1 << g; // 2^g possible permutations

    for (int i = 0; i < permutationCount; i++) {
        occurrences[i] = 0; // Initialize relevant elements of occurrences array to 0
    }

    int index;
    for (int i = 0; i <= length - g; i += g) { // Iterate through the binary array in steps of g to consider each g-sized grouping
        index = 0;
        for (int j = 0; j < g; j++) {
            index = (index*2) + array[i + j]; // Calculates the decimal value of the binary permutation
        }
        occurrences[index]++; // Add a count for the permutation with a decimal value of index
    }
}