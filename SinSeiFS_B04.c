#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/wait.h>
#define SEGMENT 1024 

static const char * directoryPath = "/home/zo/Downloads";
char prefix[6] = "AtoZ_";

void logging(char* c, int type){
	time_t currTime;
	struct tm * timeinfo;
	time ( &currTime );
	timeinfo = localtime (&currTime);
	
	FILE * logFile;
	logFile = fopen("/home/zo/SinSeiFS.log", "a");
		
    	if(type==1){//info
        	fprintf(logFile, "INFO::%d%d%d-%d:%d:%d:%s\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, c);
    	}
    	else if(type==2){ //warning
        	fprintf(logFile, "WARNING::%d%d%d-%d:%d:%d:%s\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, c);
    	}
    	fclose(logFile);
}

void logging2(const char* old, char* new) {
	FILE * logFile;
	logFile = fopen("/home/zo/fs.log", "a");
	
	fprintf(logFile, "%s -> %s\n", old, new);
    	fclose(logFile);
}

void encode1(char* strEnc1) { 
	if(strcmp(strEnc1, ".") == 0 || strcmp(strEnc1, "..") == 0)
        return;
    
    	int strLength = strlen(strEnc1);
    	for(int i = 0; i < strLength; i++) {
		if(strEnc1[i] == '/') 
            		continue;
		if(strEnc1[i]=='.')
	            break;
        
		if(strEnc1[i]>='A'&&strEnc1[i]<='Z')
	            strEnc1[i] = 'Z'+'A'-strEnc1[i];
	        if(strEnc1[i]>='a'&&strEnc1[i]<='z')
	            strEnc1[i] = 'z'+'a'-strEnc1[i];
	}
}

void decode1(char * strDec1){ 
	if(strcmp(strDec1, ".") == 0 || strcmp(strDec1, "..") == 0 || strstr(strDec1, "/") == NULL) 
        return;
    
    	int strLength = strlen(strDec1), s=0;
	for(int i = strLength; i >= 0; i--){
		if(strDec1[i]=='/')break;

		if(strDec1[i]=='.'){//nyari titik terakhir
			strLength = i;
			break;
		}
	}
	for(int i = 0; i < strLength; i++){
		if(strDec1[i]== '/'){
			s = i+1;
			break;
		}
	}
   	for(int i = s; i < strLength; i++) {
		if(strDec1[i] =='/'){
   	         continue;
   	     }
   	     if(strDec1[i]>='A'&&strDec1[i]<='Z'){
   	         strDec1[i] = 'Z'+'A'-strDec1[i];
   	     }
   	     if(strDec1[i]>='a'&&strDec1[i]<='z'){
   	         strDec1[i] = 'z'+'a'-strDec1[i];
   	     }
   	 }	
}

static  int  xmp_getattr(const char *path, struct stat *stbuf){
	char * strToEnc1 = strstr(path, prefix);
	
	if(strToEnc1 != NULL){
		decode1(strToEnc1);
    	}

	char newPath[1000];
	int result;
	sprintf(newPath,"%s%s", directoryPath, path);
	result = lstat(newPath, stbuf);
	if (result == -1)
		return -errno;
	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){ //baca directory
	char * strToEnc1 = strstr(path, prefix);
    
	if(strToEnc1 != NULL) {
        	decode1(strToEnc1);
    	}

	char newPath[1000];
	if(strcmp(path,"/") == 0){
		path=directoryPath;
		sprintf(newPath, "%s", path);
	} else 
        sprintf(newPath, "%s%s", directoryPath, path);

	int result = 0;
	struct dirent *dir;
	DIR *dp;
	(void) fi;
	(void) offset;
	dp = opendir(newPath);
	if (dp == NULL) return -errno;

	while ((dir = readdir(dp)) != NULL) { //loop yang ada di dalam directory
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = dir->d_ino;
		st.st_mode = dir->d_type << 12;
		if(strToEnc1 != NULL){
			encode1(dir->d_name); //encode yang ada di dalam directory sekarang
        	}
		
		result = (filler(buf, dir->d_name, &st, 0));
		if(result!=0) break;
	}

	closedir(dp);
	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode){ 

	char newPath[1000];
	if(strcmp(path,"/") == 0){
		path=directoryPath;
		sprintf(newPath,"%s",path);
	}
	else 
        sprintf(newPath, "%s%s",directoryPath,path);

	int result = mkdir(newPath, mode);
    	char str[100];
	sprintf(str, "CREATE::%s", path);
	logging(str,1);

	char * folderPath = strstr(path, prefix);
    
	if(folderPath != NULL) {
        	logging2(newPath, newPath);
    	}
	
	printf("%s\n",path);
	printf("%s\n",newPath);
	if (result == -1)
		return -errno;

	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev){
	char newPath[1000];
	if(strcmp(path,"/") == 0){
		path = directoryPath;
		sprintf(newPath,"%s",path);
	} 
    	else 
        	sprintf(newPath, "%s%s",directoryPath,path);
	
    	int result;

	if (S_ISREG(mode)) {
		result = open(newPath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (result >= 0)
			result = close(result);
	} else if (S_ISFIFO(mode))
		result = mkfifo(newPath, mode);
	else
		result = mknod(newPath, mode, rdev);
    	
    	char str[100];
	sprintf(str, "CREATE::%s", path);
	logging(str,1);
	if (result == -1)
		return -errno;

	return 0;
}

static int xmp_unlink(const char *path) { 
	char * strToEnc1 = strstr(path, prefix);
	if(strToEnc1 != NULL){
        	decode1(strToEnc1); 
    	}

	char newPath[1000];
	if(strcmp(path,"/") == 0){
		path=directoryPath;
		sprintf(newPath,"%s",path);
	} 
    	else 
        	sprintf(newPath, "%s%s",directoryPath,path);
    	char str[100];
	sprintf(str, "UNLINK::%s", path);
	
	char * folderPath = strstr(path, prefix);
    
	if(folderPath != NULL) {
        	logging(str,2);
    	}

	int result;
	result = unlink(newPath);
	if (result == -1)
		return -errno;

	return 0;
}

static int xmp_rmdir(const char *path) {
	char * strToEnc1 = strstr(path, prefix);
	if(strToEnc1 != NULL){
        	decode1(strToEnc1); 
    	}
	

	char newPath[1000];
	sprintf(newPath, "%s%s",directoryPath,path);
    	char str[100];
	sprintf(str, "REMOVE::%s", path);
	logging(str,2);
	int result;
	result = rmdir(newPath);
	if (result == -1)
		return -errno;

	return 0;
}

static int xmp_rename(const char *source, const char *dest) { 
	char fileSource[1000], fileDest[1000];
	sprintf(fileSource, "%s%s", directoryPath, source);
	sprintf(fileDest, "%s%s", directoryPath, dest);

    	char str[100];
	sprintf(str, "RENAME::%s::%s", source, dest);
	logging(str,1);
	
	char * folderPath = strstr(fileDest, prefix);
	if(folderPath != NULL) {
		logging2(fileSource, fileDest);
	}
	int result;
	result = rename(fileSource, fileDest);
	if (result == -1)
		return -errno;

	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi){ //open file
	char newPath[1000];
	sprintf(newPath, "%s%s", directoryPath, path);
	int result;
	result = open(newPath, fi->flags);
	if (result == -1)
		return -errno;
	close(result);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){ //read file
	
	char newPath[1000];
	sprintf(newPath, "%s%s", directoryPath, path);
	int fd;
	int result;

	(void) fi;
	fd = open(newPath, O_RDONLY);
	if (fd == -1)
		return -errno;
	result = pread(fd, buf, size, offset);
	if (result == -1)
		result = -errno;
	close(fd);
	return result;
}

static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) { //write file
	
	char newPath[1000];
	sprintf(newPath, "%s%s", directoryPath, path);
	
    	int fd;
	int result;
	(void) fi;
	fd = open(newPath, O_WRONLY);
	if (fd == -1)
		return -errno;
    	char str[100];
	sprintf(str, "WRITE::%s", path);
	logging(str,1);
	result = pwrite(fd, buf, size, offset);
	if (result == -1)
		result = -errno;
	close(fd);
	return result;
}
static struct fuse_operations xmp_oper = {

	.getattr = xmp_getattr,
	.readdir = xmp_readdir,
	.read = xmp_read,
	.mkdir = xmp_mkdir,
	.mknod = xmp_mknod,
	.unlink = xmp_unlink,
	.rmdir = xmp_rmdir,
	.rename = xmp_rename,
	.open = xmp_open,
	.write = xmp_write,

};

int  main(int  argc, char *argv[]){
	printf("halo");

	umask(0);

	return fuse_main(argc, argv, &xmp_oper, NULL);

}
