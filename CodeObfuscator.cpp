#include <bits/stdc++.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>
#include <random>
using namespace std;
mt19937 rand_main;
string generate_random_name(){
	string name;
    name += 'a'+(rand_main()%26);
    for (int i = 0; i < 9; ++i) {
        int val = rand_main()%36;
        name += (val < 10) ? ('0' + val) : ('a' + (val - 10));
    }
    return name;
}
bool is_valid_identifier(const string& token) {
    if (token.empty()) return false;
    if (!isalpha(token[0]) && token[0] != '_') return false;
    return all_of(token.begin(), token.end(), [](char c) {
        return isalnum(c) || c == '_';
    });
}
void process_content(string& content) {
    map<string, string> name_map;
    stringstream result;
    bool in_string = false;
    bool in_comment = false;
    bool in_macro = false;
    bool in_preprocessor = false;
    string current_token;
    char last_char = 0;
    char quote_char = 0;
    auto is_unescaped_quote = [&](size_t pos) {
        int backslash_count = 0;
        for (int j = (int)pos - 1; j >= 0 && content[j] == '\\'; --j) {
            backslash_count++;
        }
        // 偶数表示是未转义的引号
        return (backslash_count % 2 == 0);
    };
    for (size_t i = 0; i < content.size(); ++i) {
        char c = content[i];
        // Handle strings and character literals
        if ((c == '"' || c == '\'') && is_unescaped_quote(i)) {
            if (!in_string && !in_comment && !in_macro) {
                // 先输出已有 token
                if (!current_token.empty()) {
                    if (is_valid_identifier(current_token)) {
                        if (name_map.find(current_token) == name_map.end()) {
                            name_map[current_token] = generate_random_name();
                        }
                        result << name_map[current_token];
                    } else {
                        result << current_token;
                    }
                    current_token.clear();
                }
                in_string = true;
                quote_char = c;
            } 
            else if (in_string && c == quote_char) {
                in_string = false;
            }
            result << c;
            continue;
        }
        if (in_string) {
            result << c;
            continue;
        }
        // Handle comments
        if (!in_macro && c == '/' && i+1 < content.size()) {
            if (content[i+1] == '/') {
                in_comment = true;
            } else if (content[i+1] == '*') {
                in_comment = true;
            }
        }
        if (in_comment) {
            result << c;
            if (c == '\n' || (c == '*' && i+1 < content.size() && content[i+1] == '/')) {
                in_comment = false;
            }
            continue;
        }
        // Handle macros and preprocessor directives
        if (c == '#' && !in_string && !in_comment) {
            in_macro = true;
            in_preprocessor = true;
            result << current_token;
            current_token.clear();
        }
        if (in_macro) {
            result << c;
            if (c == '\n') {
                in_macro = false;
                in_preprocessor = false;
            }
            continue;
        }
        // Handle tokens
        if (isalnum(c) || c == '_') {
            current_token += c;
        } else {
            if (!current_token.empty()) {
                if (is_valid_identifier(current_token)) {
                    if (name_map.find(current_token) == name_map.end()) {
                        name_map[current_token] = generate_random_name();
                    }
                    result << name_map[current_token];
                } else {
                    result << current_token;
                }
                current_token.clear();
            }
            // Handle spaces and newlines
            if (isspace(c)) {
                if (c == '\n') {
                    result << c;
                } else if (last_char != ' ' && last_char != '\n') {
                    result << ' ';
                }
            } else {
                result << c;
            }
        }
        last_char = c;
    }
    cout << "map size:" << name_map.size() << endl;
    stringstream ss1;
    for (auto it = name_map.begin(); it != name_map.end(); ++it) {
        ss1 << "#define " << it->second << " " << it->first << endl;
    }
    if (!current_token.empty()) {
        if (is_valid_identifier(current_token)) {
            if (name_map.find(current_token) == name_map.end()) {
                name_map[current_token] = generate_random_name();
            }
            result << name_map[current_token];
        } else {
            result << current_token;
        }
    }
    content = ss1.str() + result.str();
}
int main(int argc, char* argv[]) {
	rand_main = mt19937(time(0));
	string s1,s2;
	cout<<"CodeObfuscator\nby zfworld hty\n";
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << endl;
        cout<<"输入文件名 输出文件名 分两行输入：\n";
        getline(cin,s1);
        getline(cin,s2);
    }
    else{
    	s1 = argv[1];
    	s2 = argv[2];
	}
    ifstream in_file(s1.c_str());
    if (!in_file) {
        cerr << "Error opening input file" << endl;
        return 1;
    }
    string content((istreambuf_iterator<char>(in_file)), 
                   istreambuf_iterator<char>());
    in_file.close();
    process_content(content);
    ofstream out_file(s2.c_str());
    if (!out_file) {
        cerr << "Error opening output file" << endl;
        return 1;
    }
    out_file << content;
    out_file.close();
    cout << "Obfuscation completed successfully" << endl;
    return 0;
}

