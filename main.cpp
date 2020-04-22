#include <iostream>
#include <fstream>
#include <vector>
#include <set>

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
            if (states.find(i) != states.end()) {
                hasNew = true;
                break;
            }
        }

        states.insert(lambdas.begin(), lambdas.end());
    } while (hasNew);

    return states;
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

int main() {
    automata a;
    read_automata(a, "input.txt");
    std::vector<std::set<int>> closures;

    for (int i = 0; i < a.states; i++) {
        std::set<int> states;
        states.insert(i);
        closures.push_back(lambdaClosure(states, a.states, a.rules));
    }

    std::vector<std::set<int>> newrules;

    newrules.resize(a.states * a.alphabet.size());

    for (int x = 0; x < a.states; x++) {
        for (int c = 0; c < a.alphabet.size(); c++) {
            std::set<int> states;
            states.insert(x);
            newrules[x * a.alphabet.size() + c] = lambdaClosure(getStates(lambdaClosure(states, a.states, a.rules), a.alphabet[c], a.states, a.rules), a.states, a.rules);
        }
    }

    return 0;
}