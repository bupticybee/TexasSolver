//
// Created by Xuefeng Huang on 2020/1/29.
//
#include "include/tools/progressbar.h"

progressbar::progressbar() :
        progress(0),
        n_cycles(0),
        last_perc(0),
        do_show_bar(true),
        update_is_called(false),
        done_char("#"),
        todo_char(" "),
        opening_bracket_char("["),
        closing_bracket_char("]") {}

progressbar::progressbar(int n, bool showbar) :
        progress(0),
        n_cycles(n),
        last_perc(0),
        do_show_bar(showbar),
        update_is_called(false),
        done_char("#"),
        todo_char(" "),
        opening_bracket_char("["),
        closing_bracket_char("]") {}

void progressbar::reset() {
    progress = 0,
            update_is_called = false;
    last_perc = 0;
    return;
}

void progressbar::set_niter(int niter) {
    if (niter <= 0) throw std::invalid_argument(
                "progressbar::set_niter: number of iterations null or negative");
    n_cycles = niter;
    return;
}

void progressbar::update() {
    return;
    if (n_cycles == 0) throw std::runtime_error(
                "progressbar::update: number of cycles not set");

    if (!update_is_called) {
        if (do_show_bar == true) {
            std::cout << opening_bracket_char;
            for (int _ = 0; _ < 50; _++) std::cout << todo_char;
            std::cout << closing_bracket_char << " 0%";
        }
        else std::cout << "0%";
    }
    update_is_called = true;

    int perc = 0;

    // compute percentage, if did not change, do nothing and return
    perc = progress*100./(n_cycles-1);
    if (perc < last_perc) return;

    // update percentage each unit
    if (perc == last_perc + 1) {
        // erase the correct  number of characters
        if      (perc <= 10)                std::cout << "\b\b"   << perc << '%';
        else if (perc  > 10 && perc < 100) std::cout << "\b\b\b" << perc << '%';
        else if (perc == 100)               std::cout << "\b\b\b" << perc << '%';
    }
    if (do_show_bar == true) {
        // update bar every ten units
        if (perc % 2 == 0) {
            // erase closing bracket
            std::cout << std::string(closing_bracket_char.size(), '\b');
            // erase trailing percentage characters
            if      (perc  < 10)               std::cout << "\b\b\b";
            else if (perc >= 10 && perc < 100) std::cout << "\b\b\b\b";
            else if (perc == 100)              std::cout << "\b\b\b\b\b";

            // erase 'todo_char'
            for (int j = 0; j < 50-(perc-1)/2; ++j) {
                std::cout << std::string(todo_char.size(), '\b');
            }

            // add one additional 'done_char'
            if (perc == 0) std::cout << todo_char;
            else           std::cout << done_char;

            // refill with 'todo_char'
            for (int j = 0; j < 50-(perc-1)/2-1; ++j) std::cout << todo_char;

            // readd trailing percentage characters
            std::cout << closing_bracket_char << ' ' << perc << '%';
        }
    }
    last_perc = perc;
    ++progress;
    std::cout << std::flush;

    return;
}

