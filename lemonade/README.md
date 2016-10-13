![Lemonade Logo](https://raw.githubusercontent.com/W3CBrasil/AI-Social/lemonade/lemonade/lemonade/app/assets/images/logo.png)
# Instalação Ubuntu 14.04
Referência: http://releases.ubuntu.com/14.04/

## Instalando os pré-requisitos
Primeiro, dê um update na sua máquina
```bash
sudo apt-get update
```
### Git
Referência: https://git-scm.com/download/linux
```bash
sudo apt-get install git-all
```
Em toda instalação o sistema irá pedir para confirmar. Somente digitar y e enter.

### Utilitários
Referências:
- https://wiki.python.org/moin/BeginnersGuide
- http://www.vim.org/docs.php
```bash
sudo apt-get install python-dev python-pip python3-pip git jq vim
```
### Bibliotecas
```bash
sudo apt-get install libboost-all-dev
sudo apt-get install libblas-dev liblapack-dev libatlas-base-dev gfortran
```
### Java 8
Referência: https://www.java.com/pt_BR/download/faq/java8.xml
```bash
sudo add-apt-repository -y ppa:webupd8team/java
sudo apt-get update
sudo apt-get -y install oracle-java8-installer
```
### ElasticSearch 2.2.1
Referência: https://www.digitalocean.com/community/tutorials/how-to-install-elasticsearch-logstash-and-kibana-elk-stack-on-ubuntu-14-04

```bash
wget -qO - https://packages.elastic.co/GPG-KEY-elasticsearch | sudo apt-key add -
echo "deb http://packages.elastic.co/elasticsearch/2.x/debian stable main" | sudo tee -a /etc/apt/sources.list.d/elasticsearch-2.x.list
sudo apt-get update
sudo apt-get -y install elasticsearch
sudo service elasticsearch restart
sudo update-rc.d elasticsearch defaults 95 10
```
### MySQL
Referência: http://dev.mysql.com/doc/refman/5.7/en/installing.html

```bash
sudo apt-get install mysql-server
sudo apt-get install libmysqlclient-dev
```

### MongoDB
Referência: https://docs.mongodb.com/manual/administration/install-on-linux/
```bash
sudo apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv EA312927
echo "deb http://repo.mongodb.org/apt/ubuntu trusty/mongodb-org/3.2 multiverse" | sudo tee /etc/apt/sources.list.d/mongodb-org-3.2.list
sudo apt-get update
sudo apt-get install -y mongodb-org
sudo service mongod start
```
### Ruby
Referência: https://www.ruby-lang.org/pt/documentation/installation/
#### rbenv
Referência: https://github.com/rbenv/rbenv
```bash
sudo apt-get update
sudo apt-get install git-core curl zlib1g-dev build-essential libssl-dev libreadline-dev libyaml-dev libsqlite3-dev sqlite3 libxml2-dev libxslt1-dev libcurl4-openssl-dev python-software-properties libffi-dev
git clone https://github.com/rbenv/rbenv.git ~/.rbenv
echo 'export PATH="$HOME/.rbenv/bin:$PATH"' >> ~/.bashrc
echo 'eval "$(rbenv init -)"' >> ~/.bashrc
exec $SHELL
git clone https://github.com/rbenv/ruby-build.git ~/.rbenv/plugins/ruby-build
echo 'export PATH="$HOME/.rbenv/plugins/ruby-build/bin:$PATH"' >> ~/.bashrc
exec $SHELL
rbenv install 2.3.1
rbenv global 2.3.1
ruby -v
```
#### bundle e rails
Referência: https://rubygems.org/pages/download
```bash
gem install bundler
gem install rails
rbenv rehash
sudo apt-get install ruby-dev
```
## Instalando a aplicação Lemonade

Para instalar a aplicação lemonade, siga os seguintes passos:

### Repositório:
```bash
git clone -b lemonade https://github.com/W3CBrasil/AI-Social.git
```
### Pré-requisitos:
```bash
sudo apt-get install python3-dev
sudo apt-get install python3-setuptools
sudo python3 $(which easy_install) pip
sudo pip install uwsgi
sudo apt-get install nginx
sudo /etc/init.d/nginx start
```
Entre na pasta:
```bash
cd AI-Social/lemonade/lemonade
```
Faça algumas configurações necessárias:
```bash
rails g mongoid:config
echo “\$elastic_search = 'http://<ip da maquina>’” >> lemonade/config/initializers/elastic_search.rb
cd -
```
Edite o arquivo settings-prod.py:
```bash
vim ai_social_analysis/ai_social_analysis/settings-prod.py
```
e modifique as variaveis:
```python
URL_LEMONADE = '<ip da maquina>' + ‘:3000’
URL_API_LEMONADE = '<ip da maquina>’
```
### Passager
Adicionando alguns certificados necessários:
```bash
sudo apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys 561F9B9CAC40B2F7
sudo apt-get install -y apt-transport-https ca-certificates
```
Passenger packages
```bash
sudo sh -c 'echo deb https://oss-binaries.phusionpassenger.com/apt/passenger trusty main > /etc/apt/sources.list.d/passenger.list'
sudo apt-get update
sudo apt-get install -y nginx-extras passenger
```
Habilite o modulo Passenger no Nginx
```bash
sudo vim /etc/nginx/nginx.conf
```
e descomente a linha
```bash
 # include /etc/nginx/passenger.conf;
```
para
```bash
 include /etc/nginx/passenger.conf;
 ```
### Deploy da ferramenta
Certifique-se que esteja na pasta "AI-Social/lemonade"
```bash
sudo ./deploy/deploy.sh
```
A instalação começa e ao decorrer da instalação muitos warnings serão apresentados.
Durante a instalação algumas intervenções são necessárias:
 1. Senha MySQL
 2. Administrador da aplicaçãoa:
  * username
  * email
  * password

Ao fim da instalação conferir se todos os algorithms resultaram em true. Caso exista falha de algum, repita o comando do deploy.
```bash
sudo ./deploy/deploy.sh
```

Habilite o inicio automatico da aplicação em background editando o /etc/rc.local
```bash
sudo vim /etc/rc.local
```
Copie a linha antes do "exit 0"
```bash
/usr/local/bin/uwsgi --emperor /etc/uwsgi/vassals --uid www-data --gid www-data --daemonize /var/log/uwsgi-emperor.log
```
Compile os assets do rails e popule o mongo com dados necessarios da aplicação:
```bash
cd /var/www/lemonade
bundle install --deployment --without development test
bundle exec rake assets:precompile db:setup RAILS_ENV=production
```
Por fim reinicie a máquina.
ex.:
```bash
sudo reboot now
```
## Instanciação de um projeto
Estando no diretório "AI-Social/lemonade/ai_social_analysis" do repositório, executar, informando os 3 parâmetros:
```bash
python3 manage.py populate_new_project <nome_do_projeto> <caminho do arquivo JSON line com os comentários> <caminho arquivo de metadados dos campos>
```
Ex.:
```bash
python3 manage.py populate_new_project dadospessoais /home/user/ai-social-analysis/data/dadospessoais-comentarios-pdfs-filtrado-mesclado.jsonl /home/user/ai-social-analysis/data/dadospessoais-comentarios-pdfs-filtrado-mesclado.properties.json
```

## Alocação de usuários em projeto
Vá para http://<ip da maquina>/admin/auth/
Clicar em Users
Adicionar usuário
Se for o caso, adicionar o usuário ao grupo de trabalho na nova base adicionada

## Referências

http://uwsgi-docs.readthedocs.io/en/latest/tutorials/Django_and_nginx.html

https://www.phusionpassenger.com/library/walkthroughs/deploy/ruby/ownserver/nginx/oss/install_passenger_main.html

https://gorails.com/setup/ubuntu/14.04

https://git-scm.com/book/en/v2/Getting-Started-Installing-Git

https://gorails.com/setup/ubuntu/14.04

http://guides.rubyonrails.org/getting_started.html












