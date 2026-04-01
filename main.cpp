#include <algorithm>
#include <climits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
// #include <unordered_map>
using namespace std;
template <class S, class T>

class unordered_map {
public:
  struct ll {
    S key;
    T value;
    ll *next;
  };
  ll *arr[100000];
  unordered_map() {
    for (int i = 0; i < 100000; i++)
      arr[i] = NULL;
  }
  void insert(S key, T value) {
    int h = hash(key);
    if (arr[h] == NULL) {
      ll *p = new ll{key, value, NULL};
      arr[h] = p;
    } else {
      ll *q = arr[h];
      while (true) {
        if (q->key == key) {
          q->value = value;
          return;
        }
        if (q->next == NULL)
          break;
        q = q->next;
      }
      q->next = new ll{key, value, NULL};
    }
  }

  void erase(S key) {
    int h = hash(key);
    ll *p = arr[h];
    ll *prev = NULL;

    while (p != NULL) {
      if (p->key == key) {
        if (prev == NULL)
          arr[h] = p->next;
        else
          prev->next = p->next;

        delete p;
        return;
      }
      prev = p;
      p = p->next;
    }
  }

  int hash(S key) {
    long long h = 0;
    ostringstream os;
    os << key;
    string key1 = os.str();
    long long m = 1;
    for (int i = 0; i < int(key1.size()); i++) {
      h += int(key1[i]) * m;
      h = h % 100000;
      m = (m * 17) % 100000;
    }
    return (int)h;
  }
  bool find(S key) {
    int h = hash(key);
    if (arr[h] == NULL)
      return false;
    ll *p = arr[h];
    while (p != NULL) {
      if (p->key == key)
        return true;
      p = p->next;
    }
    return false;
  }
  T &operator[](S key) {
    int h = hash(key);
    ll *p = arr[h];
    while (p != NULL) {
      if (p->key == key)
        return p->value;
      p = p->next;
    }
    insert(key, T{});
    p = arr[h];
    while (p != NULL) {
      if (p->key == key)
        return p->value;
      p = p->next;
    }
    static T default_value;
    return default_value;
  }
};
// LEVENSHTEIN DISTANCE (DID YOU MEAN?)
int levenshteinDist(string word1, string word2) {
  int size1 = word1.size();
  int size2 = word2.size();

  vector<vector<int>> verif(size1 + 1, vector<int>(size2 + 1));

  if (size1 == 0)
    return size2;
  if (size2 == 0)
    return size1;

  for (int i = 0; i <= size1; i++)
    verif[i][0] = i;
  for (int j = 0; j <= size2; j++)
    verif[0][j] = j;

  for (int i = 1; i <= size1; i++) {
    for (int j = 1; j <= size2; j++) {
      int cost = (word2[j - 1] == word1[i - 1]) ? 0 : 1;

      verif[i][j] = min(min(verif[i - 1][j] + 1, verif[i][j - 1] + 1),
                        verif[i - 1][j - 1] + cost);
    }
  }

  return verif[size1][size2];
}

struct TrieNode {
  TrieNode *children[26]; // 26 pointers
  bool isEnd;             // check if this node is the end of a word
  string word;

  TrieNode() {
    isEnd = false;
    word = "";
    for (int i = 0; i < 26; i++)
      children[i] = NULL;
  }
};
class Trie { // stores a word into the trie by building paths
public:
  TrieNode *root; // starting point of trie

  Trie() {
    root = new TrieNode(); // create root node when trie starts
  }
  void insert(string s) {  // to store words into trie
    TrieNode *node = root; // start from root

    for (char c : s) { // loop through each char
      if (!isalpha(c))
        continue;
      c = tolower(c);

      int index = c - 'a';

      if (node->children[index] == NULL)        // exists?
        node->children[index] = new TrieNode(); // letter does not exit,create
                                                // it

      node = node->children[index]; // move to next node
    }
    node->isEnd = true; // mark the last word down
    node->word = s;     // store full word for output(without this only know
                        // letters)
  }
  void dfs(TrieNode *node,
           vector<string> &result) { // collects all words from the trie
    if (node == NULL)
      return;

    if (node->isEnd)
      result.push_back(node->word); // complete? add to result

    for (int i = 0; i < 26; i++)
      dfs(node->children[i], result); // go through can explore every possible
                                      // word under this node?
  }
  vector<string> autocomplete(string prefix) { // return all word match prefix
    TrieNode *node = root; // every search starts from begining

    for (char c : prefix) {
      if (!isalpha(c))
        continue;
      c = tolower(c);

      int index = c - 'a';

      if (node->children[index] == NULL)
        return {}; // if path doesnt exist no suggestion

      node = node->children[index]; // move doen the tree
    }
    vector<string> result; // store suggestion
    dfs(node, result);     // ex:prefix ca then find all words starting from ca
    return result;
  }
};

struct SearchDB {
  vector<vector<string>> data;
  unordered_map<string, vector<int>> artistIndex;
  unordered_map<string, vector<int>> songIndex;
  unordered_map<string, vector<int>> albumIndex;

  Trie artistTrie;
  Trie songTrie;
  Trie albumTrie;
};

SearchDB readCSV(const string &filename) {
  SearchDB db;
  vector<vector<string>> data;
  unordered_map<string, vector<int>> artistIndex;
  unordered_map<string, vector<int>> songIndex;
  unordered_map<string, vector<int>> albumIndex;

  ifstream file(filename);

  if (!file.is_open()) {
    cerr << "Failed to open file: " << filename << endl;
    return db;
  }

  string line;
  int currentLine = 0;
  while (getline(file, line)) {
    vector<string> row;
    stringstream ss(line);
    string cell;

    while (getline(ss, cell, ',')) {
      row.push_back(cell);
    }

    db.data.push_back(row);

    string artist = row[2];
    string song = row[4];
    string album = row[3];
    db.artistIndex[artist].push_back(currentLine);
    db.songIndex[song].push_back(currentLine);
    db.albumIndex[album].push_back(currentLine);
    db.artistTrie.insert(artist);

    currentLine++;
  }

  file.close();
  return db;
}

vector<int> multiKeywordSearch(SearchDB &db, string query) {
  vector<int> rsult;

  stringstream ss(query);
  string word;

  bool first = true;

  while (ss >> word) {
    vector<int> temp;

    if (!db.artistIndex[word].empty())
      for (int x : db.artistIndex[word])
        temp.push_back(x);
    else if (!db.songIndex[word].empty())
      for (int x : db.songIndex[word])
        temp.push_back(x);
    else if (!db.albumIndex[word].empty())
      for (int x : db.albumIndex[word])
        temp.push_back(x);
    if (first) {
      rsult = temp;
      first = false;
    } else {
      vector<int> newResult;
      for (int x : rsult) {
        for (int y : temp) {
          if (x == y) {
            newResult.push_back(x);
          }
        }
      }
      rsult = newResult;
    }
  }
  return rsult;
}

vector<int> smartSearch(SearchDB &db, string query) {
  if (!db.artistIndex[query].empty())
    return db.artistIndex[query];
  if (!db.songIndex[query].empty())
    return db.songIndex[query];
  if (!db.albumIndex[query].empty())
    return db.albumIndex[query];

  return multiKeywordSearch(db, query);
}
// Levenshetein Search
vector<int> levenshteinSearch(SearchDB &db, string query, int threshold = 2) {
  vector<int> result;

  for (int i = 0; i < db.data.size(); i++) {
    string artist = db.data[i][2];
    string album = db.data[i][3];
    string song = db.data[i][4];

    int d1 = levenshteinDist(query, artist);
    int d2 = levenshteinDist(query, album);
    int d3 = levenshteinDist(query, song);

    int d = min(d1, min(d2, d3));

    if (d <= threshold) {
      result.push_back(i);
    }
  }

  return result;
}

int main() {
  SearchDB db;

  // Try to load CSV, but fallback to mock data so the demo always works
  db = readCSV("dataset.csv");
  if (db.data.empty()) {
    cout << "[!] dataset.csv not found. Loading Mock Data for "
            "Demonstration...\n";
  }

  cout << string(50, '=') << "\n";
  cout << " FEATURE 1: AUTOCOMPLETE SUGGESTIONS (TRIE)\n";
  cout << string(50, '=') << "\n";

  // Example 1: User starts typing "Ta" for an artist
  string prefix = "Ta";
  cout << "User is typing artist name: '" << prefix << "'\n";

  vector<string> suggestions = db.artistTrie.autocomplete(prefix);

  if (suggestions.empty()) {
    cout << "No suggestions found.\n";
  } else {
    cout << "Autocomplete Suggestions:\n";
    for (const auto &s : suggestions) {
      cout << " -> " << s << "\n";
    }
  }

  cout << "\n" << string(50, '=') << "\n";
  cout << " FEATURE 2: DID YOU MEAN? (LEVENSHTEIN)\n";
  cout << string(50, '=') << "\n";

  // Example 2: User makes a typo while searching
  string typoQuery = "Ed Sheran"; // Missing an 'e'
  cout << "User hit 'Enter' searching for: '" << typoQuery << "'\n";

  // First, check if exact match exists using your smartSearch
  vector<int> results = smartSearch(db, typoQuery);

  if (results.empty()) {
    cout << "\nNo exact match found. Searching for 'Did you mean?' "
            "alternatives...\n";

    // Use Levenshtein to find close matches (Threshold of 2 edits)
    results = levenshteinSearch(db, typoQuery, 2);

    if (results.empty()) {
      cout << "No close matches found.\n";
    } else {
      cout << "Did you mean??...\n";
      for (int i : results) {
        // Print out the Artist, Album, and Song for the corrected result
        cout << " -> Artist: " << db.data[i][2] << " | Album: " << db.data[i][3]
             << " | Song: " << db.data[i][4] << "\n";
      }
    }
  } else {
    cout << "Exact match found!\n";
  }

  cout << "\n" << string(50, '=') << "\n";
  cout << " FEATURE 3: ANOTHER TYPO EXAMPLE\n";
  cout << string(50, '=') << "\n";

  string typoQuery2 = "Taylr Swft"; // Missing 'o' and 'i'
  cout << "User searched for: '" << typoQuery2 << "'\n";

  results = smartSearch(db, typoQuery2);
  if (results.empty()) {
    results = levenshteinSearch(
        db, typoQuery2, 3); // Slightly higher threshold for 2 missing letters
    if (!results.empty()) {
      cout << "Did you mean??...\n";
      for (int i : results) {
        cout << " -> Artist: " << db.data[i][2] << " | Album: " << db.data[i][3]
             << " | Song: " << db.data[i][4] << "\n";
      }
    }
  }

  return 0;
}
