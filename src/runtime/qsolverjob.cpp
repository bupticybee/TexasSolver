#include "include/runtime/qsolverjob.h"


using namespace std;

void QSolverJob:: setContext(QTextEdit * textEdit){
    this->textEdit = textEdit;
}

void QSolverJob::run()
{
    QDebugStream qout(std::cout, this->textEdit);
    //this->ui->logOutput->setText(this->ui->logOutput->toPlainText() + "wow");
    std::cout << "Start Solving YesYes.." << std::endl;

    string input_file = "/Users/bytedance/TexasSolverGui/resources/text/commandline_sample_input.txt";
    string resource_dir = "/Users/bytedance/TexasSolverGui/resources";
    if(resource_dir.empty()){
        resource_dir = "./resources";
    }
    string mode = "holdem";
    if(mode.empty()){mode = "holdem";}
    if(mode != "holdem" && mode != "shortdeck")
        throw runtime_error(tfm::format("mode %s error, not in ['holdem','shortdeck']",mode));

    if(input_file.empty()) {
        CommandLineTool clt = CommandLineTool(mode,resource_dir);
        clt.startWorking();
    }else{
        cout << "EXEC FROM FILE" << endl;
        CommandLineTool clt = CommandLineTool(mode,resource_dir);
        clt.execFromFile(input_file);
    }
}
