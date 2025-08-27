//
// Created by Filipe Gomes on 14/08/2025.

//                              USO DA BIBLIOTECA LIBCURL
#include <iostream>
#include <curl/curl.h>
#include "header.h"

using namespace std;

/*                      PASSOS QUE O SMTP USA ANTES DE ENVIAR MAILS:
 *
 *  1 - tem se que conectar ao serviço SMTP numa das portas, ambas as portas 25, 587 e 465 usam SMTP,
 *  a mais usada é a 587 pois tem conexoes seguras e criptografadas, a 25 é a padrao mas pode ser bloqueada pelo ISP,
 *  a 465 é antiga e usada por alguns servidores. Com esta informação quase de ctz que a maioria dos serviços
 *  e-mail usam a porta 587.
 *
 *  2 - Envia-se o EHLO <host> que é a versão estendida do HELO, permite ao serviço e-mail saber quem
 *  sou e anunciar as capacidades do serviço de e-mail, se tiver tls (a maioria tem), o cliente decide
 *  se usa, que NO NOSSO CASO, vamos ter que usar, ja que estamos na porta 587 e todos os serviços OBRIGAM O USO.
 *  (O LIBCURL JÁ FAZ ISTO AUTOMATICAMENTE, PQ IDENTIFICA QUE É SMTP PELO URL).
 *
 *  3 - Como estamos a usar a porta 587 e o serviço de e-mail tbm, temos que enviar um STARTTLS,
 *  para iniciar a criptografia. Dps disso o canal está seguro e prossegue-se, caso n se use o STARTTLS,
 *  o serviço de e-mail fecha a conexão. (É OBRIGATÓRIO).
 *
 *  4 - Após o TLS (STARTTLS), envia-se autenticação usando o username e a senha (do gmail, hotmail, etc...)
 *
 *  5 - Defini-se o remetente, mail da pessoa que envia
 *
 *  6 - Defini-se o destinatário
 *
 *  7 - Corpo do e-mail
 *
 *  8 - fechar a sessão
 *
 *  AGORA COM OS PASSOS TODOS, É USAR A BIBLIOTECA LIBCURL.
 *  GG WP.
 *
 *  No desenvolvimento deste programa, foi bem intuitivo, o mais complicado foi fazer a função callback,
 *  que se encontra no "header.h". Após ter isso feito, apenas me faltou a option do UPLOAD "CURLOPT_UPLOAD",
 *  que é isso que permite enviar o conteudo "corpo mail, etc... oq foi definido no readfunction"
 *  ao servidor gmail, para o gmail enviar ao destinatario.
 */
static struct curl_slist* lista_destinatarios = nullptr;
Dados_Email dados;

// aqui n uso mails CC (multiplos destinatários, pq usa dados que já estão definidos ent n faz sentido), no entanto, na outra funcao uso.
void pre_definido(CURL* handle){

    const char From_mail[] = "emailstestescript@gmail.com";
    const char pass[] = "app password do gmail, só pode ser gerada depois de ter ativo 2fa";  // app password do e-mail, substitui a opção "Sign in With google". essa pass n pode ter espaços entre si quando introduzida aqui
    const char To_mail[] = "euvigandapato@gmail.com";

    // estabelecer ligação com o site, no meu caso site de mails -> (gmail, hotmail, qql site com dominio de mail)
    curl_easy_setopt(handle, CURLOPT_URL, "smtp://smtp.gmail.com:587"); // gmail

    // usa tls
    curl_easy_setopt(handle, CURLOPT_USE_SSL, CURLUSESSL_ALL);

    //username
    curl_easy_setopt(handle, CURLOPT_USERNAME, From_mail);

    //senha
    curl_easy_setopt(handle, CURLOPT_PASSWORD, pass);

    // o gmail e outros serviços e-mail exigem AUTH LOGIN especifico.
    curl_easy_setopt(handle, CURLOPT_LOGIN_OPTIONS, "AUTH=LOGIN");

    // email origem
    curl_easy_setopt(handle, CURLOPT_MAIL_FROM, From_mail);


    // email destino, é aqui que poderiamos pôr multiplos destinatarios, vamos fazer isso na funcao user_interactive
    // a struct curls_list é uma estrutura que existe na biblioteca libcurl.
    lista_destinatarios = curl_slist_append(lista_destinatarios, To_mail);
    curl_easy_setopt(handle, CURLOPT_MAIL_RCPT, lista_destinatarios); // o RCPT precisa sempre de receber um ponteiro para uma lista (struct)

    // corpo e-mail -> usa-se read para ler o formato de um email, que está numa funçao acima, o SMTP consegue distinguir o header do corpo do e-mail, pelo espaço entre o header e o corpo do e-mail.
    // curl_easy_setopt();
    const char* Form_mail = build_email(From_mail, To_mail); // constroi o header e o corpo do email que vai ser enviado, e que o readfunction extrai
    curl_easy_setopt(handle, CURLOPT_READFUNCTION, func_callback);
    dados = {Form_mail, 0}; // inicializa a estrutura com os seguintes valores
    curl_easy_setopt(handle, CURLOPT_READDATA, &dados);

    // isto diz ao libcurl "Boas, vou enviar algo via SMTP", sem isto o libcurl apenas faz a conexao TLS e estabele o handshake, mas n envia AUTH nem e-mail.
    curl_easy_setopt(handle, CURLOPT_UPLOAD, 1L); // -> foi basicamente isto que resolveu tudo. eu já tinha tudo feito, faltava apenas enviar as informações ao serviço gmail
    /*
    O que aconteceu é que CURLOPT_UPLOAD é o que diz ao libcurl:
    “Hey, vou enviar dados para o servidor”. Sem isso, ele só estabelece a conexão TLS
    e faz handshake, mas não envia nada — nem autenticação nem e-mail.

    Ou seja, mesmo com username, password e LOGIN_OPTIONS, se não pusermos CURLOPT_UPLOAD,
    o libcurl não envia o conteúdo que definiste com CURLOPT_READDATA/READFUNCTION.
    O CURLOPT_UPLOAD = 1L é que diz “agora quero enviar dados via SMTP usando o READFUNCTION que
    definiste”. Sem isso, ele apenas conecta e sai.
    */

}

// Aqui usamos CC (multiplos destinatários) INCOMPLETO
void user_interactive(CURL* handle){
    int quant_dest;

    cout << "Pretende enviar e-mail a quantos destinatários" << endl;
    cin >> quant_dest;

    // resto codigo
}

int main(){
    int dig;
    curl_global_init(CURL_GLOBAL_ALL);
    CURL* curl;

    curl = curl_easy_init();
    if(curl) {
        do {
            cout << "DIGITE O NUMERO CORRESPONDENTE AO MODO (0 ou 1) QUE QUER USAR" << endl;

            cout << "Modo 0 : Já tem os dados definidos no código, só precisa de selecionar o modo e "
                    "escrever o conteudo do mail" << endl <<
                 "MODO 1 : Tem que digitar os dados (user,pass,email origem, destinatario,etc..)"
                 "para enviar o mail" << endl;

            cin >> dig; // digita 0 ou 1

            switch (dig) {
                case 0:
                    pre_definido(curl);
                    break;

                case 1:
                    user_interactive(curl);
                    break;

                default:
                    cout << "Digitou numero errado";

            }

        } while (dig < 0 || dig > 1);

        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_perform(curl);
        curl_slist_free_all(lista_destinatarios); // free an entire curl_slist slist
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return 0;
}