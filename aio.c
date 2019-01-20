//gcc aio.c -o aio -lpthread -lrt
  #include <fcntl.h> /* open, O_RDONLY */
  #include <unistd.h> /* close */
  #include <stdio.h> /* fprintf, perror */
  #include <stdlib.h> /* EXIT_SUCCESS, EXIT_FAILURE */
  #include <string.h> /* strcmp, strcpy */
  #include <aio.h> /* aio_read, aio_error, aio_return */
  #include <errno.h> /* EINPROGRESS */
  #include <pthread.h>
  #include <time.h>

  #define FILE_SIZE 93


  struct aiocb aio;
  
  int threadNumber=0; //THREAD NUMBER taken from user.
  int bytePerThread=0; //byte per thread for writing and reading or creating source file.
  int offset=0; //read process offset for each thread. increased by bytePerThread

  char *sourceFilePath; //sourceFilePath is getting by the user,
  char *destinationFilePath;// also destinationFilePath for read and copy(write) process.
  void *createSourceFile(); //function declare for creating source file
  void *readAndWrite( void *ptr ); //function declare for read and write the source file.
  /*


 Prepared By - Mehmet Efe Kaşkaya - 
 Finished at 22:31 - 20.12.2018
  */

  int main( int argc, char *argv[] )  {

      threadNumber = atoi(argv[3]); // the third argumant in terminal is thread number
      if(threadNumber<1 || threadNumber>10)
      //if thread number is less than 1 or more than 10
      //program will exit.
      {
        printf("Wrong Thread Number entered !\n");
        printf("Program will exit !..Thread number MUST be between 1 and 10. Run Again!\n");
        printf("...\n" );
        printf(".....\n" );
        printf("......\n" );

        exit(2);
      }

      
      sourceFilePath=(char*)malloc(100); //memory allocation for source file path.
      destinationFilePath=(char*)malloc(100); //memory allocation for destination file path.

      strcpy(sourceFilePath,argv[1]); //copying the sourcefile path input on argumant 1 to sourceFilePath
      strcpy(destinationFilePath,argv[2]); //copying dest the file path input destinationFilePath

      char *compare="-"; //just a basic string for compare the source file or dest file will be created in file where located as executable file.
      char *sourceString=(char*)malloc(100); //sourceString just will be used for concat
      char *destString=(char*)malloc(100); //destString just will be used for concat

      strcpy(sourceString,"/source.txt"); //copying the constant string source.txt
      strcpy(destString,"/destination.txt"); //copying the constant string destination.txt




      if(strcmp(sourceFilePath,compare) ==0)
      {
        sourceFilePath="source.txt"; //it will be in same directory as executable file.

      }
      else
      {
        strcat(sourceFilePath,sourceString);
        //Source file path is edited as example ;/home/mefe1903/desktop/source.txt

      }
      if(strcmp(destinationFilePath,compare) ==0)
      {

        destinationFilePath="destination.txt"; //it will be in same directory where executable file located.
      }
      else
      {

        strcat(destinationFilePath,destString);
        //Source file path is edited as example ;/home/mefe1903/desktop/destination.txt

      }

      
      
      if(FILE_SIZE < 1 || FILE_SIZE > 104857600) //IF less than 1byte
      {

          printf("Wrong Byte  Size option entered !\n");
          printf("Program will exit !. \n");
          printf("You MUST choose your file size as 1 Byte to 100MB. Cannot be out of this range\n");
          printf("...\n" );
          printf(".....\n" );
          printf("......\n" );

          exit(2);


      }
      
      bytePerThread = FILE_SIZE / threadNumber; // byte for per thread for cread and write.
      int remainder = FILE_SIZE % threadNumber;
      //remainder is helping to, if file size not dividing by thread number exactly as integer.
      //if remainder is 0 , each thread will write as same as other threads
      //else, the last thread will write more than the other threads
      //other threads will write same byte.
      if(remainder != 0)
      {
        pthread_t main_thread;
        pthread_t threads[threadNumber];
        int tcr;
        int i,y=0;
        bytePerThread = (FILE_SIZE - remainder) / (threadNumber);
        int lastThread = (FILE_SIZE)-(threadNumber-1)*(bytePerThread);
        //kalan son thread remainder'ı byteperthread'e ekleyerek  yazacak
          //for example file size is 35 byte and thread number is 8 then last thread will write 7 byte
          // the others will write 4 byte

          tcr = pthread_create(&main_thread, NULL, createSourceFile,NULL);
          pthread_join(main_thread, NULL);
        
        if (tcr != 0) {
          printf("Thread creation error\n");
        }
        for (i = 0; i < threadNumber; i++)
        {
          if(i==threadNumber - 1)
          {
            //last piece is equal byteperthread plus remainder.
            //last piece will be read and copied by the last thread.
            bytePerThread = lastThread;
            tcr = pthread_create(&threads[i], NULL, readAndWrite, (void *)(intptr_t)i);
            pthread_join(threads[i], NULL);
          }
          else
          {
            tcr = pthread_create(&threads[i], NULL, readAndWrite, (void *)(intptr_t)i);
            pthread_join(threads[i], NULL);
          }


        }


      }
      else
      {
        //if filesize is dividing by the thread number and remainder is equal to 0
        //we just create source file and send all threads to readandwrite function.
        //all threads will write and read and copy same byte.
      pthread_t main_thread;  
      pthread_t threads[threadNumber];
      int tcr;
      int i,y=0;
      tcr = pthread_create(&main_thread, NULL, createSourceFile, NULL);
      pthread_join(main_thread, NULL);

  		if (tcr != 0)
      {
  			printf("Thread creation error\n");
  		}

      for (i = 0; i < threadNumber; i++)
      {
        tcr = pthread_create(&threads[i], NULL, readAndWrite, (void *)(intptr_t)i);
        pthread_join(threads[i], NULL);
      }

      }
    exit(EXIT_SUCCESS);

  }
  void *createSourceFile()
  {
    char *string = "abcdefghijklmnopqrstuvwxyz";
    size_t stringLen = 26;
    char *randomString;

    randomString = malloc(sizeof(char) * (FILE_SIZE +1));

    if (!randomString) {
        return (char*)0;
    }

    int text = 0;

    srand(time(NULL));
    for (int n = 0;n < FILE_SIZE;n++) {
        text = rand() % stringLen;
        randomString[n] = string[text];
    }

    randomString[FILE_SIZE] = '\0';
    //source file create.

    int file = open(sourceFilePath, O_CREAT|O_RDWR,0664);


    if (file == -1) {
            perror("open");
            return EXIT_FAILURE;
        }
    //copying randomstring(example : "a") to file . buffer has bytePerThread letter.
    //example if each thread write 5 byte, buffer has 5byte "safnl"...

    write(file, randomString ,FILE_SIZE);
    /*
    AIO variables.

    */
    aio.aio_fildes = file;
    aio.aio_buf = randomString;
    aio.aio_nbytes = FILE_SIZE;

    if(aio_write(&aio) == -1){
        printf("ERROR");
    }

    while(aio_error(&aio) == EINPROGRESS){ }
    //do nothing.
    //free randomstring for avoid memory leak.
    free(randomString);
    pthread_exit(NULL);
    close(file);
  }

  void *readAndWrite( void *ptr )
  {
    //read process


    int a=0;int c=100; //process percentages of starting and finishing

    int tid;
  	tid = (intptr_t) ptr;
    tid=tid+1;
    //printf ("----------Thread %d Process(Reading) ==  :   %%%d --------- \n ",tid,a);

    char *buffer = (char*)malloc(bytePerThread);
    int file = open(sourceFilePath,O_RDONLY,0664);

    if (file == -1) {
            perror("open");
            return EXIT_FAILURE;
        }

    //buffer has a size as bytePerThread and it has letter for example "aaaaa";
    memset(&aio, 0, sizeof(struct aiocb));
  	aio.aio_fildes = file;
  	aio.aio_buf = buffer;
  	aio.aio_nbytes = bytePerThread;
    aio.aio_offset = offset; //her bir thread kendi byteı kadar okuyacak,

    aio_read(&aio);
    
    
    int x;
    for(x = 0;x<bytePerThread;x++){
    
    a = (100 * x) / bytePerThread;
    printf ("----------Thread %d Process(Reading) ==  :   %%%d --------- \n ",tid,a);
    }
    a = 0;
    
    while (aio_error(&aio) == EINPROGRESS) { }
    //reading finished.

    printf ("---------Thread %d Process(Reading) ==  :   %%%d --------- \n ",tid,c);

    close(file);

    //write process(copying source.txt content to destination.txt.)


    file = open(destinationFilePath, O_CREAT|O_RDWR,0664);

    if (file == -1) {
            perror("open");
            return EXIT_FAILURE;
        }
    if(aio_write(&aio) == -1){
      printf("ERROR");
    }
    else{
    for(x = 0;x<bytePerThread;x++){
    
    a = (100 * x) / bytePerThread;
    
      printf ("---------Thread %d Process(Writing) ==  :   %%%d --------- \n ",tid,a);
    }
    }

    while(aio_error(&aio) == EINPROGRESS){ }



    free(buffer);
    close(file);

    //Offset is increasing as byteperthread for after each thread has finished write.
    offset += bytePerThread;

    printf ("---------Thread %d Process(Writing) ==  :   %%%d --------- \n ",tid,c);
    printf ("---------Thread %d Finished(Copying) Offset: %d --------- \n ",tid,offset);
    printf("\n" );

  	return EXIT_SUCCESS;
  }
  //finished.
