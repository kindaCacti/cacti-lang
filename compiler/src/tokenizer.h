#include <map>
#include <optional>
#include <string>
#include <vector>
#include "tokens.h"

struct Token{
    TokenTypes type;
    std::optional<std::string> data;
};

class Trie{
    struct Node{
        std::map<char, int> children;
        std::optional<TokenTypes> token;
    };

    std::vector<Node> nodes;
public:
    void addToken(std::string& tokenString, TokenTypes token){
        Node& currentNode = nodes[0];
        for(char c : tokenString){
            if(currentNode.children.find(c) == currentNode.children.end()){
                currentNode.children[c] = nodes.size();
                nodes.push_back({});
            }
            currentNode = nodes[currentNode.children[c]];
        }

        currentNode.token = token;
    }

    std::optional<TokenTypes> getToken(std::string& tokenString){
        Node& currentNode = nodes[0];
        for(char c : tokenString){
            if(currentNode.children.find(c) == currentNode.children.end()){
                return {};
            }
            currentNode = nodes[currentNode.children[c]];
        }

        if(!currentNode.token.has_value()){
            return {};
        }

        return currentNode.token;
    }
};

class Tokenizer : public Trie{
public:
    void loadTokens(){
        for(auto tokenData : tokenStrings){
            addToken(tokenData.tokenString, tokenData.tokenType);
        }
    }

    Tokenizer(){
        loadTokens();
    }
};