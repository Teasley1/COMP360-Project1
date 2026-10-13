/*
 * COMP 360-01 Programming Languages
 * Project 1 - LearnCompiler
 *
 * Team Members:
 * Aniya Teasley
 * Approved to work solo
 *
 * Date: October 2, 2026
 *
 * Description:
 * This program implements a lexical analyzer and
 * recursive-descent parser for the LearnCompiler grammar.
 * It reads a sample program from a file, generates lexemes
 * and tokens, and checks the syntax of the sample program.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>

using namespace std;

// Token types used by the LearnCompiler grammar
enum TokenType {
    KEYWORD,
    IDENT,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    SEMICOLON,
    ASSIGN,
    MULTIPLY,
    DIVIDE,
    END_OF_FILE,
    UNKNOWN
};

// Stores a lexeme and its token type
struct Token {
    string lexeme;
    TokenType type;
};

// Converts token types to readable names
string tokenName(TokenType type) {
    switch (type) {
        case KEYWORD:
            return "KEYWORD";

        case IDENT:
            return "IDENT";

        case LPAREN:
            return "LPAREN";

        case RPAREN:
            return "RPAREN";

        case LBRACE:
            return "LBRACE";

        case RBRACE:
            return "RBRACE";

        case SEMICOLON:
            return "SEMICOLON";

        case ASSIGN:
            return "ASSIGN";

        case MULTIPLY:
            return "MULTIPLY";

        case DIVIDE:
            return "DIVIDE";

        case END_OF_FILE:
            return "EOF";

        default:
            return "UNKNOWN";
    }
}


// ====================================================
// LEXICAL ANALYZER
// ====================================================

vector<Token> lexer(const string& input) {

    vector<Token> tokens;

    int i = 0;

    while (i < input.length()) {

        // Ignore spaces, tabs, and new lines
        if (isspace(input[i])) {
            i++;
            continue;
        }

        // Read words made of lowercase letters
        if (islower(input[i])) {

            string word = "";

            while (i < input.length() &&
                   islower(input[i])) {

                word += input[i];
                i++;
            }

            // float is the keyword in the grammar
            if (word == "float") {
                tokens.push_back({word, KEYWORD});
            }
            else {
                tokens.push_back({word, IDENT});
            }

            continue;
        }

        // Recognize symbols
        switch (input[i]) {

            case '(':
                tokens.push_back({"(", LPAREN});
                break;

            case ')':
                tokens.push_back({")", RPAREN});
                break;

            case '{':
                tokens.push_back({"{", LBRACE});
                break;

            case '}':
                tokens.push_back({"}", RBRACE});
                break;

            case ';':
                tokens.push_back({";", SEMICOLON});
                break;

            case '=':
                tokens.push_back({"=", ASSIGN});
                break;

            case '*':
                tokens.push_back({"*", MULTIPLY});
                break;

            case '/':
                tokens.push_back({"/", DIVIDE});
                break;

            default:
                tokens.push_back({
                    string(1, input[i]),
                    UNKNOWN
                });
        }

        i++;
    }

    // Mark the end of the input
    tokens.push_back({"EOF", END_OF_FILE});

    return tokens;
}


// ====================================================
// RECURSIVE-DESCENT PARSER
// ====================================================

class Parser {

private:

    vector<Token> tokens;
    int current;
    bool errorFound;

    // Returns the current token
    Token peek() {
        return tokens[current];
    }

    // Displays the first syntax error
    void syntaxError(const string& message) {

        if (!errorFound) {

            cout << "\nFirst Syntax Error: "
                 << message << endl;

            cout << "Found lexeme: \""
                 << peek().lexeme
                 << "\"" << endl;

            errorFound = true;
        }
    }

    // Checks if the current token matches
    // the expected token type
    bool match(TokenType expected) {

        if (peek().type == expected) {

            current++;
            return true;
        }

        return false;
    }


    // ------------------------------------------------
    // <expr> -> <ident> {*|/} <expr>
    //          | <ident>
    // ------------------------------------------------

    bool expr() {

        if (!match(IDENT)) {

            syntaxError(
                "Expected an identifier in expression."
            );

            return false;
        }

        // Expression can continue with * or /
        if (peek().type == MULTIPLY ||
            peek().type == DIVIDE) {

            current++;

            // Recursive call
            if (!expr()) {
                return false;
            }
        }

        return true;
    }


    // ------------------------------------------------
    // <assign> -> <ident> = <expr>
    //
    // Sample Program 1 provided with the assignment
    // places a semicolon after the assignment.
    // ------------------------------------------------

    bool assign() {

        if (!match(IDENT)) {

            syntaxError(
                "Expected identifier at beginning "
                "of assignment."
            );

            return false;
        }

        if (!match(ASSIGN)) {

            syntaxError(
                "Expected '=' after identifier."
            );

            return false;
        }

        if (!expr()) {
            return false;
        }

        // Sample Program 1 has a semicolon here
        if (!match(SEMICOLON)) {

            syntaxError(
                "Expected ';' after assignment."
            );

            return false;
        }

        return true;
    }


    // ------------------------------------------------
    // <declares> -> <keyword> <ident> ;
    //            | <keyword> <ident> ; <declares>
    // ------------------------------------------------

    bool declares() {

        if (!match(KEYWORD)) {

            syntaxError(
                "Expected 'float' in declaration."
            );

            return false;
        }

        if (!match(IDENT)) {

            syntaxError(
                "Expected identifier after 'float'."
            );

            return false;
        }

        if (!match(SEMICOLON)) {

            syntaxError(
                "Expected ';' after declaration."
            );

            return false;
        }

        // If another float appears,
        // recursively parse another declaration
        if (peek().type == KEYWORD) {

            return declares();
        }

        return true;
    }


public:

    Parser(const vector<Token>& tokenList) {

        tokens = tokenList;
        current = 0;
        errorFound = false;
    }


    // ------------------------------------------------
    // <program> ->
    // <keyword> <ident> (<keyword><ident>)
    // { <declares> <assign> }
    // ------------------------------------------------

    bool program() {

        // Function return type
        if (!match(KEYWORD)) {

            syntaxError(
                "Program must begin with 'float'."
            );

            return false;
        }

        // Function name
        if (!match(IDENT)) {

            syntaxError(
                "Expected function name after 'float'."
            );

            return false;
        }

        // (
        if (!match(LPAREN)) {

            syntaxError(
                "Expected '(' after function name."
            );

            return false;
        }

        // Parameter type
        if (!match(KEYWORD)) {

            syntaxError(
                "Expected 'float' for function parameter."
            );

            return false;
        }

        // Parameter name
        if (!match(IDENT)) {

            syntaxError(
                "Expected parameter identifier."
            );

            return false;
        }

        // )
        if (!match(RPAREN)) {

            syntaxError(
                "Expected ')' after parameter."
            );

            return false;
        }

        // {
        if (!match(LBRACE)) {

            syntaxError(
                "Expected '{' before function body."
            );

            return false;
        }

        // Parse declarations
        if (!declares()) {
            return false;
        }

        // Parse assignment
        if (!assign()) {
            return false;
        }

        // }
        if (!match(RBRACE)) {

            syntaxError(
                "Expected '}' after assignment."
            );

            return false;
        }

        // Nothing should appear after the program
        if (!match(END_OF_FILE)) {

            syntaxError(
                "Unexpected text after end of program."
            );

            return false;
        }

        return true;
    }
};


// ====================================================
// MAIN PROGRAM
// ====================================================

int main() {

    // Open the Sample Program
    ifstream inputFile("sample.txt");

    if (!inputFile) {

        cout << "Error: Could not open sample.txt"
             << endl;

        return 1;
    }

    string program = "";
    string line;

    // Read the entire Sample Program
    while (getline(inputFile, line)) {

        program += line + "\n";
    }

    inputFile.close();


    // Display the Sample Program
    cout << "Sample Program:" << endl;
    cout << "----------------" << endl;
    cout << program << endl;


    // Run the lexical analyzer
    vector<Token> tokens = lexer(program);


    // Display lexemes and tokens
    cout << "Lexemes and Tokens:" << endl;
    cout << "-------------------" << endl;

    for (const Token& token : tokens) {

        cout << token.lexeme
             << "\t\t"
             << tokenName(token.type)
             << endl;
    }


    // Run the recursive-descent parser
    Parser parser(tokens);

    cout << "\nParser Result:" << endl;
    cout << "--------------" << endl;


    if (parser.program()) {

        cout << "The Sample Program is generated "
                "by the BNF grammar"
             << endl;
    }

    else {

        cout << "The Sample Program cannot be generated "
                "by the LearnCompiler BNF Grammar"
             << endl;
    }


    return 0;
}