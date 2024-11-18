#include <iostream>
#include <vector>
#include <map>
#include <cctype>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

enum TokenType
{
    T_INT,
    T_FLOAT,
    T_DOUBLE,
    T_STRING,
    T_BOOL,
    T_CHAR,
    T_ID,
    T_NUM,
    T_IF,
    T_ELSE,
    T_RETURN,
    T_ASSIGN,
    T_PLUS,
    T_MINUS,
    T_MUL,
    T_DIV,
    T_LPAREN,
    T_RPAREN,
    T_LBRACE,
    T_RBRACE,
    T_SEMICOLON,
    T_GT,
    T_LT,
    T_EQ,
    T_NEQ,
    T_AND,
    T_OR,
    T_WHILE,
    T_FOR,
    T_EOF,
};

struct Token
{
    TokenType type;
    string value;
};

struct Symbol
{
    TokenType type;
    int scopeLevel;
    bool initialized;
};

class SymbolTable
{
private:
    map<string, Symbol> symbols;

public:
    void insert(string &name, TokenType type, int scopeLevel)
    {
        if (symbols.find(name) == symbols.end())
        {
            symbols[name] = {type, scopeLevel, false};
        }
        else
        {
            cout << "Error: Redefinition of variable '" << name << "'." << endl;
            exit(1);
        }
    }

    bool lookup(const string &name)
    {
        return symbols.find(name) != symbols.end();
    }

    Symbol &get(const string &name)
    {
        if (lookup(name))
        {
            return symbols[name];
        }
        else
        {
            cout << "Error: Variable '" << name << "' not declared." << endl;
            exit(1);
        }
    }

    void markInitialized(const string &name)
    {
        if (lookup(name))
        {
            symbols[name].initialized = true;
        }
    }
    void printTable() const
    {
        cout << "Symbol Table:" << endl;
        cout << "Name\tType\t\tScope\tInitialized" << endl;
        cout << "--------------------------------------------" << endl;
        for (const auto &entry : symbols)
        {
            const string &name = entry.first;
            const Symbol &symbol = entry.second;

            // Convert TokenType to a string for display purposes
            string typeStr;
            switch (symbol.type)
            {
            case T_INT:
                typeStr = "int";
                break;
            case T_FLOAT:
                typeStr = "float";
                break;
            case T_DOUBLE:
                typeStr = "double";
                break;
            case T_STRING:
                typeStr = "string";
                break;
            case T_BOOL:
                typeStr = "bool";
                break;
            case T_CHAR:
                typeStr = "char";
                break;
            default:
                typeStr = "unknown";
            }

            cout << name << "\t" << typeStr << "\t\t" << symbol.scopeLevel << "\t"
                 << (symbol.initialized ? "Yes" : "No") << endl;
        }
    }
};

class Lexer
{
private:
    string src;
    size_t pos;
    int line;

public:
    Lexer(const string &src)
    {
        this->src = src;
        this->pos = 0;
        this->line = 1;
    }

    vector<Token> tokenize()
    {
        vector<Token> tokens;

        while (pos < src.size())
        {
            char current = src[pos];
            if (current == '\n')
            {
                line++;
            }

            if (isspace(current))
            {
                pos++;
                continue;
            }
            // Single line comment
            if (current == '/' && peek() == '/')
            {
                pos += 2;
                while (src[pos] != '\n')
                {
                    pos++;
                }
                continue;
            }

            if (isdigit(current))
            {
                tokens.push_back(Token{T_NUM, consumeNumber()});
                continue;
            }

            if (isalpha(current))
            {
                string word = consumeWord();
                if (word == "int")
                    tokens.push_back(Token{T_INT, word});
                else if (word == "float")
                    tokens.push_back(Token{T_FLOAT, word});
                else if (word == "double")
                    tokens.push_back(Token{T_DOUBLE, word});
                else if (word == "string")
                    tokens.push_back(Token{T_STRING, word});
                else if (word == "bool")
                    tokens.push_back(Token{T_BOOL, word});
                else if (word == "char")
                    tokens.push_back(Token{T_CHAR, word});
                else if (word == "if")
                    tokens.push_back(Token{T_IF, word});
                else if (word == "else")
                    tokens.push_back(Token{T_ELSE, word});
                else if (word == "return")
                    tokens.push_back(Token{T_RETURN, word});
                else if (word == "while")
                    tokens.push_back(Token{T_WHILE, word});
                else if (word == "for")
                    tokens.push_back(Token{T_FOR, word});
                else
                    tokens.push_back(Token{T_ID, word});
                continue;
            }

            switch (current)
            {
            case '=':
                if (peek() == '=')
                {
                    pos++;
                    tokens.push_back(Token{T_EQ, "=="});
                }
                else
                {
                    tokens.push_back(Token{T_ASSIGN, "="});
                }
                break;
            case '+':
                tokens.push_back(Token{T_PLUS, "+"});
                break;
            case '-':
                tokens.push_back(Token{T_MINUS, "-"});
                break;
            case '*':
                tokens.push_back(Token{T_MUL, "*"});
                break;
            case '/':
                tokens.push_back(Token{T_DIV, "/"});
                break;
            case '(':
                tokens.push_back(Token{T_LPAREN, "("});
                break;
            case ')':
                tokens.push_back(Token{T_RPAREN, ")"});
                break;
            case '{':
                tokens.push_back(Token{T_LBRACE, "{"});
                break;
            case '}':
                tokens.push_back(Token{T_RBRACE, "}"});
                break;
            case ';':
                tokens.push_back(Token{T_SEMICOLON, ";"});
                break;
            case '>':
                tokens.push_back(Token{T_GT, ">"});
                break;
            case '<':
                tokens.push_back(Token{T_LT, "<"});
                break;
            case '&':
                if (peek() == '&')
                {
                    pos++;
                    tokens.push_back(Token{T_AND, "&&"});
                }
                else
                {
                    cout << "Unexpected character '&' at line " << line << endl;
                    exit(1);
                }
                break;
            case '|':
                if (peek() == '|')
                {
                    pos++;
                    tokens.push_back(Token{T_OR, "||"});
                }
                else
                {
                    cout << "Unexpected character '|' at line " << line << endl;
                    exit(1);
                }
                break;
            case '!':
                if (peek() == '=')
                {
                    pos++;
                    tokens.push_back(Token{T_NEQ, "!="});
                }
                else
                {
                    cout << "Unexpected character '!' at line " << line << endl;
                    exit(1);
                }
                break;
            default:
                cout << "Unexpected character: " << current << " at line " << line << endl;
                exit(1);
            }
            pos++;
        }
        tokens.push_back(Token{T_EOF, ""});
        return tokens;
    }

    char peek()
    {
        if (pos + 1 >= src.size())
            return '\0';
        return src[pos + 1];
    }

    string consumeNumber()
    {
        size_t start = pos;
        while (pos < src.size() && isdigit(src[pos]))
            pos++;
        return src.substr(start, pos - start);
    }

    string consumeWord()
    {
        size_t start = pos;
        while (pos < src.size() && isalnum(src[pos]))
            pos++;
        return src.substr(start, pos - start);
    }

    int getLineNumber() const
    {
        return line;
    }
};

class Parser
{
private:
    vector<Token> tokens;
    size_t pos;
    Lexer &lexer;
    SymbolTable symbolTable;
    int currentScopeLevel;

public:
    Parser(const vector<Token> &tokens, Lexer &lexer)
        : tokens(tokens), pos(0), lexer(lexer), currentScopeLevel(0) {}

    void parseProgram()
    {
        while (tokens[pos].type != T_EOF)
        {
            parseStatement();
        }
        cout << "Parsing completed successfully! No Syntax Error" << endl;
    }

    void parseStatement()
    {
        if (tokens[pos].type == T_INT || tokens[pos].type == T_FLOAT || tokens[pos].type == T_DOUBLE ||
            tokens[pos].type == T_STRING || tokens[pos].type == T_CHAR || tokens[pos].type == T_BOOL)
        {
            parseDeclaration();
        }
        else if (tokens[pos].type == T_ID)
        {
            parseAssignment();
        }
        else if (tokens[pos].type == T_IF)
        {
            parseIfStatement();
        }
        else if (tokens[pos].type == T_RETURN)
        {
            parseReturnStatement();
        }
        else if (tokens[pos].type == T_LBRACE)
        {
            parseBlock();
        }
        else if (tokens[pos].type == T_WHILE || tokens[pos].type == T_FOR)
        {
            parseLoop();
        }
        else
        {
            cout << "Syntax error: unexpected token " << tokens[pos].value << " at line " << lexer.getLineNumber() << endl;
            exit(1);
        }
    }

    void parseBlock()
    {
        expect(T_LBRACE);
        while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF)
        {
            parseStatement();
        }
        expect(T_RBRACE);
    }

    void parseDeclaration()
    {
        TokenType varType = tokens[pos].type;
        pos++;

        if (tokens[pos].type == T_ID)
        {
            string varName = tokens[pos].value;
            symbolTable.insert(varName, varType, currentScopeLevel);
            pos++;
            expect(T_SEMICOLON);
        }
        else
        {
            cout << "Syntax error: expected identifier after type at line " << lexer.getLineNumber() << endl;
            exit(1);
        }
    }

    void parseAssignment()
    {
        string varName = tokens[pos].value;

        if (!symbolTable.lookup(varName))
        {
            cout << "Error: Variable '" << varName << "' not declared at line " << lexer.getLineNumber() << endl;
            exit(1);
        }

        pos++;
        expect(T_ASSIGN);
        parseExpression();
        expect(T_SEMICOLON);

        symbolTable.markInitialized(varName);
    }

    void parseIfStatement()
    {
        expect(T_IF);
        expect(T_LPAREN);
        parseExpression();
        expect(T_RPAREN);
        parseStatement();
        if (tokens[pos].type == T_ELSE)
        {
            expect(T_ELSE);
            parseStatement();
        }
    }

    void parseLoop()
    {
        if (tokens[pos].type == T_WHILE)
        {
            expect(T_WHILE);
            expect(T_LPAREN);
            parseExpression();
            expect(T_RPAREN);
            parseStatement();
        }
        else if (tokens[pos].type == T_FOR)
        {
            expect(T_FOR);
            expect(T_LPAREN);
            parseStatement();
            parseExpression();
            expect(T_SEMICOLON);
            parseStatement();
            expect(T_RPAREN);
            parseStatement();
        }
    }

    void parseReturnStatement()
    {
        expect(T_RETURN);
        parseExpression();
        expect(T_SEMICOLON);
    }

    void parseExpression()
    {
        parseTerm();
        while (tokens[pos].type == T_PLUS || tokens[pos].type == T_MINUS || tokens[pos].type == T_GT ||
               tokens[pos].type == T_LT || tokens[pos].type == T_EQ || tokens[pos].type == T_NEQ ||
               tokens[pos].type == T_AND || tokens[pos].type == T_OR)
        {
            pos++;
            parseTerm();
        }
    }

    void parseTerm()
    {
        parseFactor();
        while (tokens[pos].type == T_MUL || tokens[pos].type == T_DIV)
        {
            pos++;
            parseFactor();
        }
    }

    void parseFactor()
    {
        if (tokens[pos].type == T_NUM || tokens[pos].type == T_ID)
        {
            pos++;
        }
        else if (tokens[pos].type == T_LPAREN)
        {
            expect(T_LPAREN);
            parseExpression();
            expect(T_RPAREN);
        }
        else
        {
            cout << "Syntax error: expected number or identifier at line " << lexer.getLineNumber() << endl;
            exit(1);
        }
    }

    void expect(TokenType expected)
    {
        if (tokens[pos].type == expected)
        {
            pos++;
        }
        else
        {
            cout << "Syntax error: expected token " << expected << " at line " << lexer.getLineNumber() << endl;
            exit(1);
        }
    }
    SymbolTable &getSymbolTable()
    {
        return symbolTable;
    }
};

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        cout << "Usage: " << argv[0] << " <source-file>" << endl;
        return 1;
    }

    ifstream file(argv[1]);
    if (!file)
    {
        cout << "Error: Cannot open file " << argv[1] << endl;
        return 1;
    }

    string input((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

    Lexer lexer(input);
    vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens, lexer);
    parser.parseProgram();

    parser.getSymbolTable().printTable();

    return 0;
}
