/*******************************************************************************
 * Name          : commonwordfinder.cpp
 * Author        : Jonathan Pavlik
 * Date          : April 26, 2014
 * Version       : 0.1
 * Pledge        : I pledge my honor that I have abided by the Stevens Honor System -- Jonathan Pavlik
 ******************************************************************************/
#include "rbtree.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>
#include <cmath>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>

using namespace std;

int main(int argc, char const *argv[]) {
  //check proper arg number
  if(argc > 3 || argc < 2) {
    cerr << "Usage: ./commonwordfinder <filename> [limit]";
    return 1;
  }

  //if third exists change number of owrds requested otherwise it is 10
  int common_word_number = 10;
  if(argc == 3) {
    istringstream iss(argv[2]);
    if((!(iss>>common_word_number) || common_word_number <= 0) ) {
      cerr << "Error: Invalid limit \'" << argv[2] << "\' received.";
      return 1;
    }
  }

  const char *filepath = argv[1];
  int fd = open(filepath, O_RDONLY, (mode_t)0600);
  //check if valid file
  if(fd == -1) {
    cerr << "Error: Cannot open file \'" << argv[1] << "\' for input.";
    return 1;
  }
  struct stat fileInfo = {0};
  if (fstat(fd, &fileInfo) == -1) {
    close(fd);
    cerr << "Error getting the file size" << endl;
    return 1;
  }
  if (fileInfo.st_size == 0) {
    cerr << "Error: File is empty, nothing to do" << endl;
    return 1;
  }
  char *map = (char*)mmap(0, fileInfo.st_size, PROT_READ, MAP_SHARED, fd, 0);
  if (map == MAP_FAILED) {
    close(fd);
    cerr << "Error mmapping the file" << endl;
    return 1;
  }

  //create rb tree
  RedBlackTree<string, int> *words = new RedBlackTree<string, int>();
  //string to hold word and char to look at each character
  string word;
  char c;
  for(int i = 0; i < fileInfo.st_size; ++i) {
    //convert upper to lower
    c = map[i];
    //check if other valid character
    if( (c >= 97 && c <= 122) || c == 45 || c == 39 ) {
      word.push_back(c);
    }
    //otherwise
    else if(word.length() != 0) {
      auto found = words->find(word);
      if(found == words->end()) {
        words->insert(word, 1);
      }
      else {
        found->second++;
      }
      word = "";
    }
    if(c >= 65 &&  c <= 90) {
      word.push_back(c^' ');
    }
  }
  //make sure to close file
  close(fd);


  pair<string, int> **most_common = new pair<string, int>*[words->size()];
  auto it = words->begin();
  //the number of words
  size_t j = 0;
  //while nodes in the tree put them in the array into the proper spot leftmost being highest occurance
  while (it != words->end()) {
    if ((j < (unsigned)common_word_number && ++j) || it->second > most_common[j-1]->second) {
      auto temp(*it);
      size_t place = upper_bound(most_common, most_common + j-1, &temp, [](auto a, auto b) { return a->second > b->second; }) - most_common;
      pair<string, int>* pair_ptr;
      if ((int) j-1 < common_word_number) {
        pair_ptr = new pair<string, int>(*it);
      } else {
        pair_ptr = most_common[j-1];
        *pair_ptr = *it;
      }
      for (int i = j-2; i >= (int) place; i--) {
        most_common[i+1] = most_common[i];
      }
      most_common[place] = pair_ptr;
    }
    ++it;
  }

  //print them out
  cout << "Total unique words: " << words->size() << endl;
  int wordNumber = 1;
  int space = floor(log10(j)) + 1;

  unsigned int wordmax = 0;
  for(int i = 0; i < (signed)j; ++i) {
    if(most_common[i]->first.size() > wordmax) wordmax = most_common[i]->first.size();
  }

  for(int i = 0; i < int(j); ++i, ++wordNumber) {
    cout << setw(space) << right << wordNumber << ". " << setw(wordmax) << left << most_common[i]->first << " " << most_common[i]->second << endl;
  }

  for (int i = 0; i < (int) j; ++i) {
    delete most_common[i];
  }
  delete[] most_common;
  delete words;
  return 0;
}
