# soal-shift-sisop-modul-4-B04-2021
## Anggota Kelompok ##

NRP | NAMA
------------- | -------------
05111940000139  | Zahrotul Adillah
05111940000111  | Evelyn Sierra
05111940000059  | Dido Fabian Fayed

## Soal 1
Di suatu jurusan, terdapat admin lab baru yang super duper gabut, ia bernama Sin. Sin baru menjadi admin di lab tersebut selama 1 bulan. Selama sebulan tersebut ia bertemu orang-orang hebat di lab tersebut, salah satunya yaitu Sei. Sei dan Sin akhirnya berteman baik. Karena belakangan ini sedang ramai tentang kasus keamanan data, mereka berniat membuat filesystem dengan metode encode yang mutakhir. Berikut adalah filesystem rancangan Sin dan Sei :
```txt
NOTE : 
Semua file yang berada pada direktori harus ter-encode menggunakan Atbash cipher(mirror).
Misalkan terdapat file bernama kucinglucu123.jpg pada direktori DATA_PENTING
“AtoZ_folder/DATA_PENTING/kucinglucu123.jpg” → “AtoZ_folder/WZGZ_KVMGRMT/pfxrmtofxf123.jpg”
Note : filesystem berfungsi normal layaknya linux pada umumnya, Mount source (root) filesystem adalah directory /home/[USER]/Downloads, dalam penamaan file ‘/’ diabaikan, dan ekstensi tidak perlu di-encode.
Referensi : https://www.dcode.fr/atbash-cipher
```

- Jika sebuah direktori dibuat dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.
- Jika sebuah direktori di-rename dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.
- Apabila direktori yang terenkripsi di-rename menjadi tidak ter-encode, maka isi direktori tersebut akan terdecode.
- Setiap pembuatan direktori ter-encode (mkdir atau rename) akan tercatat ke sebuah log. Format : /home/[USER]/Downloads/[Nama Direktori] → /home/[USER]/Downloads/AtoZ_[Nama Direktori]
- Metode encode pada suatu direktori juga berlaku terhadap direktori yang ada di dalamnya.(rekursif)

### Penyelesaian
Fungsi yang dipakai untuk encode dan decode sesuai request soal adalah sebagai berikut :
```c
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
```
```c
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
```
Fungsi diatas berjalan sepanjang nama file dengan mengabaikan `/` dan akan berhenti saat bertemu dengan `.` dimana `.` berada sebelum ekstensi. Selain itu, akan di-endoce atau decode. Sedangkan fungsi untuk poin d yaitu menulis log adalah sebagai berikut :
```c
void logging2(const char* old, char* new) {
	FILE * logFile;
	logFile = fopen("/home/zo/fs.log", "a");
	
	fprintf(logFile, "%s -> %s\n", old, new);
    	fclose(logFile);
}
```
Fungsi di atas akan dipanggil ketika folder berawalan `AtoZ_` terbentuk baik saat membuat(mkdir) maupun saat rename folder menjadi berawalan `AtoZ`. Fungsi tersebut menuliskan log sesuai dengan format `/home/[USER]/Downloads/[Nama Direktori] → /home/[USER]/Downloads/AtoZ_[Nama Direktori]`, jika file log belum ada dalam direktori, file log akan dibuat baru menuliskannya pada file tersebut. Setelah menulis pada file, tidak lupa diclose kembali. 

## Soal 2
Selain itu Sei mengusulkan untuk membuat metode enkripsi tambahan agar data pada komputer mereka semakin aman. Berikut rancangan metode enkripsi tambahan yang dirancang oleh Sei
- Jika sebuah direktori dibuat dengan awalan “RX_[Nama]”, maka direktori tersebut akan menjadi direktori terencode beserta isinya dengan perubahan nama isi sesuai kasus nomor 1 dengan algoritma tambahan ROT13 (Atbash + ROT13).
- Jika sebuah direktori di-rename dengan awalan “RX_[Nama]”, maka direktori tersebut akan menjadi direktori terencode beserta isinya dengan perubahan nama isi sesuai dengan kasus nomor 1 dengan algoritma tambahan Vigenere Cipher dengan key “SISOP” (Case-sensitive, Atbash + Vigenere).
- Apabila direktori yang terencode di-rename (Dihilangkan “RX_” nya), maka folder menjadi tidak terencode dan isi direktori tersebut akan terdecode berdasar nama aslinya.
- Setiap pembuatan direktori terencode (mkdir atau rename) akan tercatat ke sebuah log file beserta methodnya (apakah itu mkdir atau rename).
- Pada metode enkripsi ini, file-file pada direktori asli akan menjadi terpecah menjadi file-file kecil sebesar 1024 bytes, sementara jika diakses melalui filesystem rancangan Sin dan Sei akan menjadi normal. Sebagai contoh, Suatu_File.txt berukuran 3 kiloBytes pada directory asli akan menjadi 3 file kecil yakni:
```txt
Suatu_File.txt.0000
Suatu_File.txt.0001
Suatu_File.txt.0002
```
Ketika diakses melalui filesystem hanya akan muncul Suatu_File.txt

## Soal 3
Karena Sin masih super duper gabut akhirnya dia menambahkan sebuah fitur lagi pada filesystem mereka. 
- Jika sebuah direktori dibuat dengan awalan “A_is_a_”, maka direktori tersebut akan menjadi sebuah direktori spesial.
- Jika sebuah direktori di-rename dengan memberi awalan “A_is_a_”, maka direktori tersebut akan menjadi sebuah direktori spesial.
- Apabila direktori yang terenkripsi di-rename dengan menghapus “A_is_a_” pada bagian awal nama folder maka direktori tersebut menjadi direktori normal.
- Direktori spesial adalah direktori yang mengembalikan enkripsi/encoding pada direktori “AtoZ_” maupun “RX_” namun masing-masing aturan mereka tetap berjalan pada direktori di dalamnya (sifat recursive  “AtoZ_” dan “RX_” tetap berjalan pada subdirektori).
- Pada direktori spesial semua nama file (tidak termasuk ekstensi) pada fuse akan berubah menjadi lowercase insensitive dan diberi ekstensi baru berupa nilai desimal dari binner perbedaan namanya.
Contohnya jika pada direktori asli nama filenya adalah “FiLe_CoNtoH.txt” maka pada fuse akan menjadi “file_contoh.txt.1321”. 1321 berasal dari biner 10100101001.

## Soal 4
Untuk memudahkan dalam memonitor kegiatan pada filesystem mereka Sin dan Sei membuat sebuah log system dengan spesifikasi sebagai berikut.
- Log system yang akan terbentuk bernama “SinSeiFS.log” pada direktori home pengguna (/home/[user]/SinSeiFS.log). Log system ini akan menyimpan daftar perintah system call yang telah dijalankan pada filesystem.
- Karena Sin dan Sei suka kerapian maka log yang dibuat akan dibagi menjadi dua level, yaitu INFO dan WARNING.
- Untuk log level WARNING, digunakan untuk mencatat syscall rmdir dan unlink.
- Sisanya, akan dicatat pada level INFO.
Format untuk logging yaitu:
```[Level]::[dd][mm][yyyy]-[HH]:[MM]:[SS]:[CMD]::[DESC :: DESC]```
Level : Level logging, dd : 2 digit tanggal, mm : 2 digit bulan, yyyy : 4 digit tahun, HH : 2 digit jam (format 24 Jam),MM : 2 digit menit, SS : 2 digit detik, CMD : System Call yang terpanggil, DESC : informasi dan parameter tambahan
```txt
INFO::28052021-10:00:00:CREATE::/test.txt
INFO::28052021-10:01:00:RENAME::/test.txt::/rename.txt
```
### Penyelesaian 
Fungsi untuk pembuatan log adalah sebagai berikut :
```c
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
```
Fungsi di atas akan dipanggil pada fungsi `mkdir`, `mknode`, `unlink`, `rmdir`, `rename`, dan `write`. Fungsi tersebut akan menuliskan info/warning, timestamp saat operasi dilakukan, beserta keterangannya. Untuk warning akan digunakan saat `unlink` dan `rmdir`, sedangkan yang lainnya merupakan info. Berikut contoh pemanggilan pada fungsi `rmdir` dan `rename`.
```c
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
```
```c
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
```


