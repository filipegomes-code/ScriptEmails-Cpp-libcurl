//
// Created by Filipe Gomes on 14/08/2025.
//

#ifndef PROJETOS_DE_IDEIAS_INOVADORAS_HEADER_H
#define PROJETOS_DE_IDEIAS_INOVADORAS_HEADER_H

// funcao com formato da mensagem de e-mail, necessaria para o SMTP saber distinguir entre header e corpo
const char* build_email(const char* from, const char* to) {
    static char payload_text[1024];
    snprintf(payload_text, sizeof(payload_text),
             "To: %s\r\n"
             "From: %s\r\n"
             "Subject: O meu nome\r\n"
             "\r\n"
             /*Body*/     "Olá chamo-me macaco\r\n"
            , to, from);

    return payload_text;
}

// estrutura usada na funcao de callback para guardar a mensagem e a quantidade de bytes copiados
struct Dados_Email{
    const char* buffer;
    size_t bytes_read;
};

// funcao de callback , que a readfunction chama sempre que ainda n copiou tudo
size_t func_callback(char* ptr, size_t size, size_t nitems, void* userp){

    struct Dados_Email* dados = (struct Dados_Email*)userp;

    size_t bytes_now = strlen(dados->buffer) - dados->bytes_read; // quantidade bytes que faltam enviar
    size_t buffer_libcurl = size * nitems; // o buffer do libcurl da funcao readfunction tem um limite de bytes, se for menor que o conteudo do formato do email, é enviado esse limite que o buffer suporta no maximo

    //precauçao extra, size e nitems no libcurl nunca sao 0, mas eles usaram no exemplo deles, tbm pus
    if(size == 0 || nitems == 0 || buffer_libcurl < 1)
        return 0;

    const char* data = &dados->buffer[dados->bytes_read];

    // se a quantidade de bytes do formato da mensagem for superior ao limite do buffer do libcurl, enviamos a quantidade de bytes
    if(bytes_now > buffer_libcurl) {
        bytes_now = buffer_libcurl;
        memcpy(ptr, data , bytes_now);
        dados->bytes_read += bytes_now;

        return bytes_now;
    }else{
        memcpy(ptr, data, bytes_now);
        dados->bytes_read += bytes_now;

        return bytes_now;
    }

}



#endif //PROJETOS_DE_IDEIAS_INOVADORAS_HEADER_H
