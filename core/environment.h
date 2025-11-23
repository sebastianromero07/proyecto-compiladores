#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

template <typename T>
class Environment {
private:
    vector<unordered_map<string, T>> ribs;

    int search_rib(const string& var) const {
        for (int i = ribs.size() - 1; i >= 0; i--) {
            if (ribs[i].count(var)) {
                return i;
            }
        }
        return -1;
    }

public:
    Environment() {
        add_level(); // Nivel global
    }

    void clear() {
        ribs.clear();
    }

    void add_level() {
        ribs.push_back(unordered_map<string, T>());
    }

    void add_var(const string& var, const T& value) {
        if (!ribs.empty()) {
            ribs.back()[var] = value;
        }
    }

    void add_var(const string& var) {
        if (!ribs.empty()) {
            ribs.back()[var] = T();
        }
    }

    bool remove_level() {
        if (!ribs.empty()) {
            ribs.pop_back();
            return true;
        }
        return false;
    }

    bool update(const string& x, const T& v) {
        int rib = search_rib(x);
        if (rib >= 0) {
            ribs[rib][x] = v;
            return true;
        }
        return false;
    }

    bool check(const string& x) const {
        return search_rib(x) >= 0;
    }

    T lookup(const string& x) const {
        int rib = search_rib(x);
        if (rib >= 0) {
            return ribs[rib].at(x);
        }
        return T();
    }
};

#endif // ENVIRONMENT_H