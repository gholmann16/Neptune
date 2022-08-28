int self(int argc, char* argv[]) {
    
  IupOpen(&argc, &argv);
  
  IupMessage("Hello World 1", "Hello world from IUP.");
  
  IupClose();
  return EXIT_SUCCESS;
}