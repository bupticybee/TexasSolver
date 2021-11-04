//
// Created by Xuefeng Huang on 2020/1/29.
//

#ifndef TEXASSOLVER_PROGRESSBAR_H
#define TEXASSOLVER_PROGRESSBAR_H
// The MIT License (MIT)
//
// Copyright (c) 2019 Luigi Pertoldi
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//
// ============================================================================
//  ___   ___   ___   __    ___   ____  __   __   ___    __    ___
// | |_) | |_) / / \ / /`_ | |_) | |_  ( (` ( (` | |_)  / /\  | |_)
// |_|   |_| \ \_\_/ \_\_/ |_| \ |_|__ _)_) _)_) |_|_) /_/--\ |_| \
//
// Very simple progress bar for c++ loops with internal running variable
//
// Author: Luigi Pertoldi
// Created: 3 dic 2016
//
// Notes: The bar must be used when there's no other possible source of output
//        inside the for loop
//

#include <iostream>
#include <string>

class progressbar {

public:
    // default destructor
    ~progressbar()                             = default;

    // delete everything else
    progressbar           (progressbar const&) = delete;
    progressbar& operator=(progressbar const&) = delete;
    progressbar           (progressbar&&)      = delete;
    progressbar& operator=(progressbar&&)      = delete;

    // default constructor, must call set_niter later
    progressbar();
    progressbar(int n, bool showbar=true);

    // reset bar to use it again
    void reset();
    // set number of loop iterations
    void set_niter(int iter);
    // chose your style
    void set_done_char(const std::string& sym) {done_char = sym;}
    void set_todo_char(const std::string& sym) {todo_char = sym;}
    void set_opening_bracket_char(const std::string& sym) {opening_bracket_char = sym;}
    void set_closing_bracket_char(const std::string& sym) {closing_bracket_char = sym;}
    // to show only the percentage
    void show_bar(bool flag = true) {do_show_bar = flag;}
    // main function
    void update();

private:
    int progress;
    int n_cycles;
    int last_perc;
    bool do_show_bar;
    bool update_is_called;

    std::string done_char;
    std::string todo_char;
    std::string opening_bracket_char;
    std::string closing_bracket_char;
};

#endif //TEXASSOLVER_PROGRESSBAR_H
