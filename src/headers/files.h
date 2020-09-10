#ifndef FILES_H
#define FILES_H

#include "events.h"

namespace deng {

    enum dengWriteModes {
        DENG_WRITEMODE_REWRITE = 0,
        DENG_WRITEMODE_FROM_END = 1
    };

    class FileManager {
    private:
        std::ifstream *p_file_in;
        std::ofstream *p_file_out;

    public:
        void getFileContents(const std::string &file_name, std::vector<char> *p_char_output_vector, std::vector<std::string> *p_string_output_vector);
        void writeToFile(const std::string &file_name, const std::string &line_contents, const uint32_t &write_mode);

        template<typename T>
        T getConfVal(const std::string &conf, const std::string &file_name) {
            std::vector<std::string> file_contents;
            this->getFileContents(file_name, nullptr, &file_contents);
            int32_t line_index = 0;

            for(std::string &line : file_contents) {
                if(line.find(conf) == 0) {
                    std::string value_str;
                    int8_t index;

                    if(static_cast<int>(line.find("#")) == -1 && static_cast<int>(line.find("=")) == -1) {
                        ERR(DENG_CONF_PARSING_ERROR_MSG(file_name) + DENG_GENERAL_PARSING_ERROR_LINE_MSG(std::to_string(line_index)));
                    }

                    else {
                        index = static_cast<int8_t>(line.find("=")) + 1;
                        while(line[index] == ' ' || line[index] == '"') index++;

                        for(; index < line.size() && line[index] != ' ' && line[index] != '#' && line[index] != '"'; index++) {
                            value_str += line[index];
                        }
                    }

                    T i;
                    LOG("ValStr: \"" + value_str + "\"");
                    if(typeid(i).name() == typeid(bool).name()) {
                        if(value_str == "true") return true;
                        else if(value_str == "false") return false; 
                    }

                    else if(typeid(i).name() == typeid(float).name()) {
                        return std::stof(value_str);
                    }

                    else if(typeid(i).name() == typeid(int).name()) {
                        return std::stoi(value_str);
                    }                       
                }
                line_index++;
            }
            T i;
            return i;
        }
    };

    struct EditorEnvironmentConf {
        float environment_color_r;
        float environment_color_g;
        float environment_color_b;

        dengBool show_grid;
        float grid_height;
        float grid_width;
        
        float grid_line_color_r;
        float grid_line_color_g;
        float grid_line_color_b;
    };

    struct EditorCameraConf {
        float movement_x;
        float movement_y;
        float movement_z;

        float mouse_movement_x;
        float mouse_movement_y;

        float fov;
    };
}

#endif