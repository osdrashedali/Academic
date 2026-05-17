#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
using namespace std;

// keyword list (small version)
string keywords[] = {"int","float","char","return","if","else","while","for"};

set<string> K, I, N, S, O;

// check keyword
bool isKeyword(string w) {
    for (string k : keywords)
        if (w == k) return true;
    return false;
}

// check number
bool isNumber(string s) {
    if (s.empty()) return false;
    for (char c : s)
        if (!isdigit(c) && c != '.') return false;
    return true;
}

// main function
int main() {

    ifstream file("input.c");
    if (!file) {
        cout << "File not found!";
        return 0;
    }

    stringstream ss;
    ss << file.rdbuf();

    string code = ss.str();
    string token = "";

    for (char ch : code) {

        // space or symbol break token
        if (isspace(ch) || ch=='(' || ch==')' || ch=='{' || ch=='}' || ch==';' || ch==',' || ch=='=') {

            if (!token.empty()) {
                if (isKeyword(token)) K.insert(token);
                else if (isNumber(token)) N.insert(token);
                else I.insert(token);

                token = "";
            }

            if (!isspace(ch))
                S.insert(string(1, ch));
        }
        else {
            token += ch;
        }
    }

    // print result
    cout << "Keywords: ";
    for (auto x : K) cout << x << " ";

    cout << "\nIdentifiers: ";
    for (auto x : I) cout << x << " ";

    cout << "\nNumbers: ";
    for (auto x : N) cout << x << " ";

    cout << "\nSymbols: ";
    for (auto x : S) cout << x << " ";

    return 0;
}
