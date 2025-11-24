#pragma once
#include "defines.h"
#include <forward_list>
#include <stack>
#include <span>
#include <string>
#include <map>
#include <unordered_map>
#include <deque>
#include <cctype>

using CharSpan = std::span<const char>;

enum class SymbolKind {
    Number,
    Variable,
    Constant,
    Identifier,
    Operator,
    Function,
    LParen,
    RParen,
    Comma
};

struct Symbol {
    SymbolKind kind{};
    double value{};
    std::string text{};
    int priority{};
};

NO_DISCARD
bool IsIdentStart(unsigned char c) {
    return std::isalpha(c) || c == '_';
}

NO_DISCARD
bool IsIdentChar(unsigned char c) {
    return std::isalnum(c) || c == '_';
}

NO_DISCARD
bool IsNumberChar(unsigned char c) {
    return std::isdigit(c) || c == '.';
}

NO_DISCARD
int GetOperatorPriority(const std::string& op) {
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/") return 2;
    if (op == "^") return 3;
    return 0;
}

NO_DISCARD
bool IsRightAssociative(const std::string& op) {
    return op == "^";
}

NO_DISCARD
bool IsFunctionName(const std::string& name,
                    const std::deque<std::string>& functions) {
    for (const std::string& f : functions)
        if (f == name) return true;
    return false;
}

NO_DISCARD
std::forward_list<Symbol> LexSymbols(
        std::string& input,
        const std::map<std::string, double>& constants,
        const std::unordered_map<std::string, double>& variables,
        const std::deque<std::string>& functions) {

    std::forward_list<Symbol> result;
    auto it = result.before_begin();

    const char* data = input.data();
    const usize n = input.size();
    usize i = 0;

    while (i < n) {
        unsigned char ch = static_cast<unsigned char>(data[i]);
        if (std::isspace(ch)) {
            ++i;
            continue;
        }

        Symbol s{};
        usize start = i;

        if (data[i] == '(') {
            ++i;
            s.kind = SymbolKind::LParen;
            s.text.assign(1, '(');
            it = result.insert_after(it, s);
            continue;
        }
        if (data[i] == ')') {
            ++i;
            s.kind = SymbolKind::RParen;
            s.text.assign(1, ')');
            it = result.insert_after(it, s);
            continue;
        }
        if (data[i] == ',') {
            ++i;
            s.kind = SymbolKind::Comma;
            s.text.assign(1, ',');
            it = result.insert_after(it, s);
            continue;
        }

        if (i + 1 < n) {
            char c1 = data[i];
            char c2 = data[i + 1];
            bool twoCharOp =
                (c1 == '>' && c2 == '=') ||
                (c1 == '<' && c2 == '=') ||
                (c1 == '=' && c2 == '=') ||
                (c1 == '!' && c2 == '=') ||
                (c1 == '+' && c2 == '+') ||
                (c1 == '-' && c2 == '-') ||
                (c1 == '&' && c2 == '&') ||
                (c1 == '|' && c2 == '|');

            if (twoCharOp) {
                i += 2;
                s.kind = SymbolKind::Operator;
                s.text.assign(data + start, data + i);
                s.priority = 0;
                it = result.insert_after(it, s);
                continue;
            }
        }

        if (IsNumberChar(ch)) {
            ++i;
            while (i < n && IsNumberChar(static_cast<unsigned char>(data[i]))) ++i;
            s.kind = SymbolKind::Number;
            s.text.assign(data + start, data + i);
            s.value = std::stod(s.text);
            it = result.insert_after(it, s);
            continue;
        }

        if (IsIdentStart(ch)) {
            ++i;
            while (i < n && IsIdentChar(static_cast<unsigned char>(data[i]))) ++i;
            s.text.assign(data + start, data + i);

            auto itC = constants.find(s.text);
            if (itC != constants.end()) {
                s.kind = SymbolKind::Constant;
                s.value = itC->second;
                it = result.insert_after(it, s);
                continue;
            }

            auto itV = variables.find(s.text);
            if (itV != variables.end()) {
                s.kind = SymbolKind::Variable;
                s.value = itV->second;
                it = result.insert_after(it, s);
                continue;
            }

            if (IsFunctionName(s.text, functions)) {
                s.kind = SymbolKind::Function;
                s.priority = 4;
                it = result.insert_after(it, s);
                continue;
            }

            s.kind = SymbolKind::Identifier;
            it = result.insert_after(it, s);
            continue;
        }

        if (data[i] == '+' || data[i] == '-' || data[i] == '*' ||
            data[i] == '/' || data[i] == '^' || data[i] == '=' ||
            data[i] == '<' || data[i] == '>' || data[i] == '!' ||
            data[i] == '&' || data[i] == '|' || data[i] == ';' ||
            data[i] == '%') {
            char c = data[i];
            ++i;
            s.kind = SymbolKind::Operator;
            s.text.assign(1, c);
            s.priority = GetOperatorPriority(s.text);
            it = result.insert_after(it, s);
            continue;
        }

        throw std::runtime_error("Nieznany znak w leksie");
    }

    return result;
}

NO_DISCARD
std::forward_list<Symbol> InfixToPostfix(
        const std::forward_list<Symbol>& infix) {

    std::forward_list<Symbol> output;
    auto outIt = output.before_begin();
    std::stack<Symbol> st;

    for (const Symbol& s : infix) {
        switch (s.kind) {
        case SymbolKind::Number:
        case SymbolKind::Variable:
        case SymbolKind::Constant:
        case SymbolKind::Identifier:
            outIt = output.insert_after(outIt, s);
            break;

        case SymbolKind::Function:
            st.push(s);
            break;

        case SymbolKind::Operator: {
            while (!st.empty()) {
                const Symbol& t = st.top();
                if (t.kind != SymbolKind::Operator &&
                    t.kind != SymbolKind::Function)
                    break;

                bool higher =
                    t.priority > s.priority ||
                    (t.priority == s.priority &&
                     !IsRightAssociative(s.text));

                if (!higher) break;

                outIt = output.insert_after(outIt, t);
                st.pop();
            }
            st.push(s);
            break;
        }

        case SymbolKind::LParen:
            st.push(s);
            break;

        case SymbolKind::RParen: {
            bool found = false;
            while (!st.empty()) {
                if (st.top().kind == SymbolKind::LParen) {
                    found = true;
                    st.pop();
                    break;
                }
                outIt = output.insert_after(outIt, st.top());
                st.pop();
            }
            if (!found) throw std::runtime_error("Brakujący '('");
            if (!st.empty() && st.top().kind == SymbolKind::Function) {
                outIt = output.insert_after(outIt, st.top());
                st.pop();
            }
            break;
        }

        case SymbolKind::Comma:
            while (!st.empty() && st.top().kind != SymbolKind::LParen) {
                outIt = output.insert_after(outIt, st.top());
                st.pop();
            }
            if (st.empty()) throw std::runtime_error("Zły przecinek");
            break;
        }
    }

    while (!st.empty()) {
        if (st.top().kind == SymbolKind::LParen ||
            st.top().kind == SymbolKind::RParen)
            throw std::runtime_error("Niedomknięty nawias");
        outIt = output.insert_after(outIt, st.top());
        st.pop();
    }

    return output;
}
