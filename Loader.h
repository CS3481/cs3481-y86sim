
class Loader
{
   private:
      bool loaded;        //set to true if a file is successfully loaded into memory
      std::ifstream inf;  //input file handle
      bool checkInputFile();
      void loadline(char *x);
      bool hasErrors(char *x);
   public:
      Loader(int argc, char * argv[]);
      bool isLoaded();
};
