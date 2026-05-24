#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_FILES 32
#define MAX_TOTAL_SIZE 209715200

typedef struct
{
    char name[256];
    int permission;
    long size;
} FileInfo;

long get_file_size(const char *filename)
{
    struct stat st;

    if (stat(filename, &st) != 0)
        return -1;

    return st.st_size;
}

void build_archive(int argc, char *argv[])
{
    int i;
    int file_count = 0;
    long total_size = 0;
    char *output_name = "a.sau";

    for (i = 2; i < argc; i++)
    {
        if (strcmp(argv[i], "-o") == 0)
        {
            if (i + 1 < argc)
                output_name = argv[i + 1];
            break;
        }

        file_count++;
    }

    if (file_count > MAX_FILES)
    {
        printf("En fazla 32 giris dosyasi olabilir!\n");
        return;
    }

    FILE *out = fopen(output_name, "w");
    if (out == NULL)
    {
        printf("Arsiv dosyasi olusturulamadi!\n");
        return;
    }

    fprintf(out, "0000000000");

    char info[10000] = "";
    char temp[512];

    for (i = 2; i < 2 + file_count; i++)
    {
        struct stat st;
        long size = get_file_size(argv[i]);

        if (size < 0)
        {
            printf("%s giris dosyasinin formati uyumsuzdur!\n", argv[i]);
            fclose(out);
            return;
        }

        total_size += size;

        if (total_size > MAX_TOTAL_SIZE)
        {
            printf("Giris dosyalarinin toplam boyutu 200 MB'i gecemez!\n");
            fclose(out);
            return;
        }

        stat(argv[i], &st);

        sprintf(temp, "|%s,%o,%ld|", argv[i], st.st_mode & 0777, size);
        strcat(info, temp);
    }

    long info_size = strlen(info);
    char size_text[11];

    sprintf(size_text, "%010ld", info_size);

    fseek(out, 0, SEEK_SET);
    fprintf(out, "%s", size_text);

    fseek(out, 10, SEEK_SET);
    fprintf(out, "%s", info);

    for (i = 2; i < 2 + file_count; i++)
    {
        FILE *in = fopen(argv[i], "r");

        if (in == NULL)
        {
            printf("%s giris dosyasinin formati uyumsuzdur!\n", argv[i]);
            fclose(out);
            return;
        }

        int ch;
        while ((ch = fgetc(in)) != EOF)
        {
            fputc(ch, out);
        }

        fclose(in);
    }

    fclose(out);
    printf("Dosyalar birlestirildi.\n");
}

void extract_archive(int argc, char *argv[])
{
    if (argc < 3 || argc > 4)
    {
        printf("Arsiv dosyasi uygunsuz veya bozuk!\n");
        return;
    }

    char *archive_name = argv[2];
    char *directory = ".";

    if (argc == 4)
        directory = argv[3];

    FILE *archive = fopen(archive_name, "r");
    if (archive == NULL)
    {
        printf("Arsiv dosyasi uygunsuz veya bozuk!\n");
        return;
    }

    char size_text[11];
    size_text[10] = '\0';

    if (fread(size_text, 1, 10, archive) != 10)
    {
        printf("Arsiv dosyasi uygunsuz veya bozuk!\n");
        fclose(archive);
        return;
    }

    long info_size = atol(size_text);

    if (info_size <= 0)
    {
        printf("Arsiv dosyasi uygunsuz veya bozuk!\n");
        fclose(archive);
        return;
    }

    char *info = (char *)malloc(info_size + 1);
    if (info == NULL)
    {
        printf("Bellek hatasi!\n");
        fclose(archive);
        return;
    }

	if (fread(info, 1, info_size, archive) != (size_t)info_size)
    {
        printf("Arsiv dosyasi uygunsuz veya bozuk!\n");
        free(info);
        fclose(archive);
        return;
    }

    info[info_size] = '\0';

    FileInfo files[MAX_FILES];
    int file_count = 0;

    char *token = strtok(info, "|");

    while (token != NULL)
    {
        if (strlen(token) > 0)
        {
            if (file_count >= MAX_FILES)
            {
                printf("Arsiv dosyasi uygunsuz veya bozuk!\n");
                free(info);
                fclose(archive);
                return;
            }

            if (sscanf(token, "%255[^,],%o,%ld",
                       files[file_count].name,
                       &files[file_count].permission,
                       &files[file_count].size) != 3)
            {
                printf("Arsiv dosyasi uygunsuz veya bozuk!\n");
                free(info);
                fclose(archive);
                return;
            }

            file_count++;
        }

        token = strtok(NULL, "|");
    }

    mkdir(directory, 0777);

    int i;
    for (i = 0; i < file_count; i++)
    {
        char path[512];

        if (strcmp(directory, ".") == 0)
            sprintf(path, "%s", files[i].name);
        else
            sprintf(path, "%s/%s", directory, files[i].name);

        FILE *out = fopen(path, "w");
        if (out == NULL)
        {
            printf("Dosya acilamadi: %s\n", path);
            free(info);
            fclose(archive);
            return;
        }

        long j;
        for (j = 0; j < files[i].size; j++)
        {
            int ch = fgetc(archive);

            if (ch == EOF)
            {
                printf("Arsiv dosyasi uygunsuz veya bozuk!\n");
                fclose(out);
                free(info);
                fclose(archive);
                return;
            }

            fputc(ch, out);
        }

        fclose(out);
        chmod(path, files[i].permission);
    }

    printf("%s dizininde dosyalar acildi.\n", directory);

    free(info);
    fclose(archive);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Kullanim:\n");
        printf("./tarsau -b dosyalar -o arsiv.sau\n");
        printf("./tarsau -a arsiv.sau dizin\n");
        return 1;
    }

    if (strcmp(argv[1], "-b") == 0)
    {
        build_archive(argc, argv);
    }
    else if (strcmp(argv[1], "-a") == 0)
    {
        extract_archive(argc, argv);
    }
    else
    {
        printf("Gecersiz parametre!\n");
    }

    return 0;
}
