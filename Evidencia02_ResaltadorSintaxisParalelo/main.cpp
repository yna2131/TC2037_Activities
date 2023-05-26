// C++ Standard Libraries
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <regex>
#include <chrono>
#include <thread>

// Constants
std::string html_header = R"(
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta charset='UTF-8'>
        <meta http-equiv='X-UA-Compatible' content='IE=edge'>
        <meta name='viewport' content='width=device-width, initial-scale=1.0'>
        <title>Csharp Highlighter</title>
        <style>
            body {
                background-color: #434973; /* darker night sky */
                color: #FFE5D4; /* warm light */
                font-family: 'Courier New', Courier, monospace;
            }

            .keyword {
                color: #F67E7D; /* vibrant red from castle */
            }

            .comment {
                color: #85CDC8; /* brighter version of Sophie's dress */
            }

            .string {
                color: #FBE274; /* vibrant gold */
            }

            .number {
                color: #FFABAB; /* softer red */
            }

            .operator {
                color: #FF726F; /* fiery red of Calcifer */
            }

            .punctuation {
                color: #FBE274; /* vibrant gold */
            }

            .identifier {
                color: #A7E0E0; /* sky blue */
            }

            .preprocessor {
                color: #F67E7D; /* vibrant red from castle */
            }

            .exception {
                color: #FF726F; /* fiery red of Calcifer */
            }

            .datatypes {
                color: #FFABAB; /* softer red */
            }
        </style>
    </head>
    <body>
    <pre><code>
)";

std::string html_footer = R"(
    </code></pre>
    </body>
    </html>
)";

// Function to tokenize a line
std::string escape_regex_metacharacters(const std::string &token)
{
    static const std::string metacharacters = R"([-[\]{}()*+?.,\^$|#\s])";
    return std::regex_replace(token, std::regex(metacharacters), R"(\$&)");
}

std::string tokenize_line(const std::string &line)
{
    std::regex token_regex(R"(\/\/[^\n]*|\"(?:[^\"\\]|\\.)*\"|\b\d+(\.\d+)?\b|\s*([-+*\/%><=|&!^~]+)\s*|[()\[\]{}.,;:]|\b(class|struct|interface|enum|namespace)\b|\b(try|catch|finally|throw)\b|[()\[\]{}]|\\b([a-zA-Z_][a-zA-Z0-9_]*)\\b|\b(public|private|protected|internal|static|readonly|const|abstract|sealed|virtual|override|extern|unsafe|volatile)\b|\b(abstract|as|base|bool|break|byte|case|catch|char|checked|class|const|continue|decimal|default|delegate|do|double|else|enum|event|explicit|extern|false|finally|fixed|float|for|foreach|goto|if|implicit|in|int|interface|internal|is|lock|long|namespace|new|null|object|operator|out|override|params|private|protected|public|readonly|ref|return|sbyte|sealed|short|sizeof|stackalloc|static|string|struct|switch|this|throw|true|try|typeof|uint|ulong|unchecked|unsafe|ushort|using|using static|virtual|void|volatile|while)\b|\b(bool|byte|char|decimal|double|float|int|long|object|sbyte|short|string|uint|ulong|ushort)\b(\[\])?)");

    std::string highlighted_line = line;

    auto token_begin = std::sregex_iterator(line.begin(), line.end(), token_regex);
    auto token_end = std::sregex_iterator();

    for (std::sregex_iterator it = token_begin; it != token_end; ++it)
    {
        std::smatch match = *it;
        std::string token = match.str();

        // Determine the appropriate span class based on the token type
        std::string span_class;
        if (token[0] == '/')
            span_class = "comment";
        else if (token[0] == '\"')
            span_class = "string";
        else if (std::isdigit(token[0]))
            span_class = "number";
        else if (std::regex_match(token, std::regex(R"(\s*([-+*\/%><=|&!^~]+)\s*)")))
            span_class = "operator";
        else if (std::regex_match(token, std::regex(R"([()\[\]{}.,;:])")))
            span_class = "punctuation";
        else if (std::regex_match(token, std::regex(R"(\b(class|struct|interface|enum|namespace)\b)")))
            span_class = "keyword";
        else if (std::regex_match(token, std::regex(R"(\b(try|catch|finally|throw)\b)")))
            span_class = "exception";
        else if (std::regex_match(token, std::regex(R"(\\b(if|else|else if)\\b)")))
            span_class = "keyword";
        else if (std::regex_match(token, std::regex(R"(\\b([a-zA-Z_][a-zA-Z0-9_]*)\\b)")))
            span_class = "identifier";
        else if (std::regex_match(token, std::regex(R"(\b(public|private|protected|internal|static|readonly|const|abstract|sealed|virtual|override|extern|unsafe|volatile)\b)")))
            span_class = "keyword";
        else if (std::regex_match(token, std::regex(R"(\b(abstract|as|base|bool|break|byte|case|catch|char|checked|class|const|continue|decimal|default|delegate|do|double|else|enum|event|explicit|extern|false|finally|fixed|float|for|foreach|goto|if|implicit|in|int|interface|internal|is|lock|long|namespace|new|null|object|operator|out|override|params|private|protected|public|readonly|ref|return|sbyte|sealed|short|sizeof|stackalloc|static|string|struct|switch|this|throw|true|try|typeof|uint|ulong|unchecked|unsafe|ushort|using|using static|virtual|void|volatile|while)\b)")))
            span_class = "keyword";
        else if (std::regex_match(token, std::regex(R"(\b(bool|byte|char|decimal|double|float|int|long|object|sbyte|short|string|uint|ulong|ushort)\b(\[\])?)")))
            span_class = "datatypes";
        else
            span_class = ""; // No span class for unrecognized token types

        // Wrap the token with the appropriate span class
        highlighted_line = std::regex_replace(highlighted_line,
                                              std::regex(escape_regex_metacharacters(token)),
                                              "<span class=\"" + span_class + "\">" + token + "</span>");
    }

    return highlighted_line;
}

// Main function
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cout << "Usage: " << argv[0] << " input directory, single thread output directory, "
                  << "multithread output directory" << std::endl;
        return 1;
    }

    std::string input_directory = argv[1];
    if (!std::filesystem::exists(input_directory))
    {
        std::cout << "Input directory does not exist" << std::endl;
        return 1;
    }

    std::vector<std::string> filenames;

    for (const auto &entry : std::filesystem::directory_iterator(input_directory))
    {
        std::string filename = entry.path().filename().string();
        std::string extension = filename.substr(filename.find_last_of(".") + 1);

        if (extension == "cs")
            filenames.push_back(filename);
    }

    std::string output_dir_single = argv[2];
    std::filesystem::create_directory(output_dir_single);

    std::string output_dir_multi = argv[3];
    std::filesystem::create_directory(output_dir_multi);

    // Single thread
    auto start = std::chrono::high_resolution_clock::now();

    for (const auto &filename : filenames)
    {
        std::ifstream input_file(input_directory + "/" + filename);
        std::ofstream output_file(output_dir_single + "/" + filename + ".html");

        output_file << html_header;

        std::string line;
        while (std::getline(input_file, line))
        {
            output_file << tokenize_line(line) << std::endl;
        }

        output_file << html_footer;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Single thread: " << elapsed.count() << " seconds" << std::endl;

    // Multi thread
    start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    threads.resize(std::thread::hardware_concurrency());

    for (int i = 0; i < threads.size(); ++i)
    {
        threads[i] = std::thread(
            [&filenames, &input_directory, &output_dir_multi, i, &threads]()
            {
            for (int j = i; j < filenames.size(); j += threads.size())
            {
                std::ifstream input_file(input_directory + "/" + filenames[j]);
                std::ofstream output_file(output_dir_multi + "/" + filenames[j] + ".html");

                output_file << html_header;

                std::string line;
                while (std::getline(input_file, line))
                {
                    output_file << tokenize_line(line) << std::endl;
                }

                output_file << html_footer;
            } });
    }

    for (auto &thread : threads)
        if (thread.joinable())
            thread.join();

    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;

    std::cout << "Multi thread: " << elapsed.count() << " seconds" << std::endl;

    return 0;
}