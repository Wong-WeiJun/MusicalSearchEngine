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
int main() {
  SearchDB db = readCSV("dataset.csv");
  string input;
  string query;
  // row[0]  row[1]    row[2]    row[3]      row[4]      row[5]      row[6]
  // row[7] index   track_id  artists   album_name  track_name  popularity
  // duration_ms   explicit row[8]        row[9]  row[10]   row[11]    row[12]
  // row[13]       row[14]       row[15] danceability  energy  key loudness mode
  // speechiness   acousticness  instrumentalness row[16]     row[17]   row[18]
  // row[19]          row[20] liveness    valence   tempo     time_signature
  // track_genre

  cout << "\nType Which Artist You Want To Search(Starts with 'Jen'):\n";
  cin >> input;
  vector<string> suggestions = db.artistTrie.autocomplete(input);

  cout << "\nSuggestions: \n";
  for (auto &s : suggestions) {
    cout << s << endl;
  }

  if (suggestions.size() == 1) {
    query = suggestions[0];
    cout << "\nAuto-Selected: " << query << endl;
  } else {
    cout << "\nType What You Want To Search (Artist / Song / Album): \n";
    cin.ignore();
    getline(cin, query);
  }

  vector<int> results = smartSearch(db, query);

  cout << "\nSearch Results: \n";
  for (int i : results) {
    for (int j = 0; j < db.data[i].size(); j++) {
      if (j == 1)
        continue;
      cout << db.data[i][j] << "\t";
    }
    cout << endl;
  }

  // for (const auto &row : db.data) {
  //   if (row[2] == "Gen Hoshino") {
  //     for (const auto &cell : row) {
  //       cout << cell << "\t";
  //     }
  //     cout << endl;
  //   }
  // }

  return 0;
}
