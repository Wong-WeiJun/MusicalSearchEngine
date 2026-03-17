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
          q->value = value; // Update existing key
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
          arr[h] = p->next; // Removing the head
        else
          prev->next = p->next; // Removing from middle/end

        delete p; // Use delete, not free!
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
    long long m = 1; // was int — overflows after a few iterations
    for (int i = 0; i < int(key1.size()); i++) {
      h += int(key1[i]) * m;
      h = h % 100000;
      m = (m * 17) % 100000; // keep m bounded too
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
    // Key not found — insert default and return reference to it
    insert(key, T{});
    // Now find and return the reference
    p = arr[h];
    while (p != NULL) {
      if (p->key == key)
        return p->value;
      p = p->next;
    }
    // Should never reach here
    static T default_value;
    return default_value;
  }
};
struct SearchDB {
  vector<vector<string>> data;
  unordered_map<string, vector<int>> artistIndex;
  unordered_map<string, vector<int>> songIndex;
  unordered_map<string, vector<int>> albumIndex;
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

    currentLine++;
  }

  file.close();
  return db;
}

int main() {
  SearchDB db = readCSV("dataset.csv");
  // row[0]  row[1]    row[2]    row[3]      row[4]      row[5]      row[6]
  // row[7] index   track_id  artists   album_name  track_name  popularity
  // duration_ms   explicit row[8]        row[9]  row[10]   row[11]    row[12]
  // row[13]       row[14]       row[15] danceability  energy  key loudness mode
  // speechiness   acousticness  instrumentalness row[16]     row[17]   row[18]
  // row[19]          row[20] liveness    valence   tempo     time_signature
  // track_genre

  for (const auto &row : db.data) {
    if (row[2] == "Gen Hoshino") {
      for (const auto &cell : row) {
        cout << cell << "\t";
      }
      cout << endl;
    }
  }

  return 0;
}
