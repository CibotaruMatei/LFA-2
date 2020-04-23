#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>
#include <iterator>

typedef struct {
    int states = 0;
    std::string alphabet;
    int init_state = 0;
    std::set<int> final_states;
    std::vector<std::vector<char>> rules;
}automata;

bool belongsTo(char i, std::vector<char> const& v) {
    return std::find(v.begin(), v.end(), i) != v.end();
}

std::set<int> getStates(std::set<int> const& states, char c, int maxStates, std::vector<std::vector<char>> const& rules) {
    std::set<int> newstates;

    for (auto& state : states) {
        for (int i = 0; i < maxStates; i++) {
            if (belongsTo(c, rules[state * maxStates + i])) {
                newstates.insert(i);
            }
        }
    }

    return newstates;
}

std::set<int> lambdaClosure(std::set<int> states, int maxStates, std::vector<std::vector<char>> const& rules) {
    std::set<int> lambdas;
    bool hasNew;

    do {
        lambdas = getStates(states, '$', maxStates, rules);
        hasNew = false;
        for (auto& i : lambdas) {
            if (states.find(i) == states.end()) {
                hasNew = true;
                break;
            }
        }

        states.insert(lambdas.begin(), lambdas.end());
    } while (hasNew);

    return states;
}

std::vector<std::set<int>> getClosures(automata a) {
    std::vector<std::set<int>> closures;
    for (int i = 0; i < a.states; i++) {
        std::set<int> states;
        states.insert(i);
        closures.push_back(lambdaClosure(states, a.states, a.rules));
    }
    return closures;
}

void read_automata(automata& a, std::string input) {
    std::fstream f;
    f.open(input, std::ios::in);

    f >> a.states;

    int n; //counter

    f >> n; //characters in alphabet
    for (int i = 0; i < n; i++) {
        char c;
        f >> c;
        a.alphabet += c;
    }

    f >> a.init_state;

    f >> n; //nr of final states
    for (int i = 0; i < n; i++) {
        int state;
        f >> state;
        a.final_states.insert(state);
    }

    a.rules.resize(a.states * a.states);

    f >> n; //nr of transitions
    for (int i = 0; i < n; i++) {
        int x, y;
        char c;
        f >> x;
        f >> c;
        f >> y;

        a.rules[x * a.states + y].push_back(c);
    }

    f.close();
}

void rulesConvert(automata &a, std::vector<std::set<int>> input) {
    a.rules.clear();
    a.rules.resize(a.states*a.states);
    for (int x = 0; x < a.states; x++) {
        for (int c = 0; c < a.alphabet.size(); c++) {
            for (auto& y : input[x * a.alphabet.size() + c]) {
                a.rules[x * a.states + y].push_back(a.alphabet[c]);
            }
        }
    }
}

void removeNode(automata& a, int node) {
    if (a.final_states.find(node) != a.final_states.end())
        a.final_states.erase(a.final_states.find(node));

    a.states--;

    std::vector<std::vector<char>> oldrules;
    a.rules.clear();
    a.rules.resize(a.states * a.states);
    for (int i = 0; i < a.states; i++) {
        for (int j = 0; j < a.states; j++) {
            int pos = 0;
            if (i < node) pos += i * a.states;
            else pos += (i + 1) * a.states;
            if (j < node) pos += j;
            else pos += j + 1;

            a.rules[i * a.states + j] = oldrules[pos];
        }
    }
}

std::ostream& operator<<(std::ostream& os, const std::set<int>& s) {
    for (auto& i : s) {
        os << i << " ";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<std::set<int>>& s) {
    for (int i = 0; i < s.size(); i++) {
        os << i << ": " << s[i] << "\n";
    }
    return os << "\n";
}

int main() {
    automata a;
    read_automata(a, "input.txt");
    std::vector<std::set<int>> closures = getClosures(a);
    std::cout << closures;
    std::vector<std::set<int>> newrules;

    newrules.resize(a.states * a.alphabet.size());

    for (int x = 0; x < a.states; x++) {
        for (int c = 0; c < a.alphabet.size(); c++) {
            std::set<int> states;
            states.insert(x);
            newrules[x * a.alphabet.size() + c] = lambdaClosure(getStates(lambdaClosure(states, a.states, a.rules), a.alphabet[c], a.states, a.rules), a.states, a.rules);
        }
    }
    std::cout << newrules;
    rulesConvert(a, newrules);

    for (int i = 0; i < a.states; i++) {
        std::set<int> c = closures[i];
        std::set<int> intersect;
        std::set_intersection(c.begin(), c.end(), a.final_states.begin(), a.final_states.end(), std::inserter(intersect, intersect.begin()));
        std::cout << intersect << "\n";
        if (intersect.size() > 0) a.final_states.insert(i);
    }

    std::cout << "\n";

    for (int i = 0; i < a.states; i++) {
        for (int j = i + 1; j < a.states; j++) {
            bool found = false;
            for (int k = 0; k < a.alphabet.size(); k++) {
                std::set<int> di = newrules[i * a.alphabet.size() + k];
                std::set<int> dj = newrules[j * a.alphabet.size() + k];

                if (di != dj) {
                    found = true;
                    break;
                }
            }

            if (!found && !(a.final_states.find(i) != a.final_states.end() ^ a.final_states.find(j) != a.final_states.end())) {
                std::cout << i << " " << j << "\n";
                removeNode(a, i);
                i--;
                break;
            }
        }
    }

    return 0;
}