%module PokerSolver
%{
#include "runtime/PokerSolver.cpp"
%}
%include "runtime/PokerSolver.cpp"
/*
class PokerSolver{
    public:
        PokerSolver();
        PokerSolver(string,string,string,int);
        void load_game_tree(string);
        void train(
            string,
            string,
            string,
            string,
            int,
            int,
            string
        );
    private:
};
 */
