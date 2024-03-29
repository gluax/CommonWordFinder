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

using namespace std;

void insert(string key, RedBlackTree<string, int> *words) {
  auto found = words->find(key);
  if(found == words->end()) {
    words->insert(key, 1);
  }
  else {
    found->second++;
  }
}

int main(int argc, char const *argv[]) {
  if(argc > 3 || argc < 2) {
    cerr << "Usage: ./commonwordfinder <filename> [limit]";
    return 1;
  }

  int common_word_number = 10;
  if(argc == 3) {
    istringstream iss(argv[2]);
    if((!(iss>>common_word_number) || common_word_number <= 0) ) {
      cerr << "Error: Invalid limit \'" << argv[2] << "\' received.";
      return 1;
    }
  }

  ifstream file(argv[1]);
  if(!file) {
    cerr << "Error: Cannot open file \'" << argv[1] << "\' for input.";
    return 1;
  }

  file.exceptions(ifstream::badbit);
  RedBlackTree<string, int> *words = new RedBlackTree<string, int>();
  string line, token, word;
  try {
    while(getline(file, line)) {
      stringstream ss(line);
      while(ss) {
        if(ss.peek() >= 65 &&  ss.peek() <= 90) {
          word.push_back(ss.get()+32);
        }
        else if( (ss.peek() >= 97 && ss.peek() <= 122) || ss.peek() == 45 || ss.peek() == 39 ) {
          word.push_back(ss.get());
        }
        else if(ss.peek() == 32) {
          //cout << "word: " << word << " length: " << word.length() << endl;
          if (word.length() != 0) insert(word, words);
          word = "";
          ss.get();
        }
        else if(ss.peek() == -1) {
          //cout << "word: " << word << " length: " << word.length() << endl;
          if (word.length() != 0) insert(word, words);
          word = "";
          break;
        }
        else {
          ss.get();
        }
      }

    }

    file.close();
  } catch(const ifstream::failure &f) {
    cerr << "Error: An I/O error occurred reading '" << argv[1] << "'.";
    return 1;
  }

  vector<pair<string, int> > most_common;
  most_common.reserve(common_word_number);
  auto it = words->begin();
  for (int i = 0; i < common_word_number && it != words->end(); i++, ++it) {
    most_common.push_back(*it);
  }
  stable_sort(most_common.begin(), most_common.end(), [](auto a, auto b) { return a.second > b.second; });
  for (; it != words->end(); ++it) {
    if (it->second > most_common.back().second) {
      most_common.pop_back();
      most_common.insert(upper_bound(most_common.begin(), most_common.end(), *it, [](auto a, auto b) { return a.second > b.second; }), *it);
    }
  }


  cout << "Total unique words: " << words->size() << endl;
  int wordNumber = 1;
  int space;
  if(most_common.size() < (unsigned)common_word_number) space = floor(log10(most_common.size())) + 1;
  else space = floor(log10(common_word_number)) + 1;
  if (space == 1) --space;
  int i = 1;
  unsigned int wordmax = 0;
  for(auto it=most_common.begin(); it!=most_common.end(); ++it, ++i) {
    if(it->first.length() > wordmax) wordmax = it->first.length();
  }

  for(auto it=most_common.begin(); it!=most_common.end(); ++it, ++wordNumber) {
    cout << setw(space) << wordNumber << ". " << it->first << setw(wordmax-it->first.length() + floor(log10(it->second)) + 2) << setfill(' ') << it->second << endl;
  }
  delete words;
  return 0;
}
