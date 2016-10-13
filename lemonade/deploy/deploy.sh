#!/bin/bash




echo "+ Configurando ElasticSearch"

sudo grep -v '^#' deploy/elasticsearch_config.txt >> /etc/elasticsearch/elasticsearch.yml


echo "+ Configurando nginx"

sudo rm /etc/nginx/sites-enabled/default
sudo cp deploy/ai_social_analysis_nginx.conf /etc/nginx/sites-available/
sudo cp deploy/lemonade_nginx.conf /etc/nginx/sites-available/
sudo ln -s /etc/nginx/sites-available/ai_social_analysis_nginx.conf /etc/nginx/sites-enabled/
sudo ln -s /etc/nginx/sites-available/lemonade_nginx.conf /etc/nginx/sites-enabled/

echo "+ Configurando Rails"
cd lemonade
bundle install
cd -

echo "+ Configurando www-data"
sudo cp -r ai_social_analysis /var/www/
sudo cp -r lemonade /var/www
sudo chown -R www-data:www-data /var/www/ai_social_analysis
sudo chown -R www-data:www-data /var/www/lemonade
sudo chmod -R a+w /var/www


echo "+ Configurando uwsgi"

sudo mkdir /etc/uwsgi
sudo mkdir /etc/uwsgi/vassals
sudo ln -s /var/www/ai_social_analysis/ai_social_analysis_uwsgi.ini /etc/uwsgi/vassals/


echo "+ Configurando MySQL"
echo "  - Digite a senha de root do MySQL"
sudo mysql -u root -p < deploy/create_database.mysql

echo "+ Configurando Aplicação"
cd /var/www/lemonade
bundle install --deployment --without development test
bundle exec rake assets:precompile db:setup RAILS_ENV=production
cd -


echo "+ Instalando dependências do Python"


cd /var/www/ai_social_analysis
sudo pip install -r requirements.txt
sudo pip3 install -r requirements.txt





echo "+ Configurando Aplicação"

cd ai_social_analysis
sudo ln -s settings-prod.py settings.py
cd -

sudo python3 manage.py migrate
sudo python3 manage.py collectstatic --noinput

echo "  - Digite o nome de usuário, email e senha para o administrador do sistema"
sudo python3 manage.py createsuperuser

sudo python3 manage.py update_algorithms


#python3 manage.py populate_new_project dadosparciais /home/ubuntu/ai-social-analysis/data/dadospessoais-comentarios-pdfs-filtrado-mesclado.jsonl /home/ubuntu/ai-social-analysis/data/dadospessoais-comentarios-pdfs-filtrado-mesclado.properties.json

