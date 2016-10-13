#!/bin/bash

rm -rf /var/www/ai_social_analysis

cp -r ai_social_analysis /var/www/
chown -R www-data:www-data /var/www/ai_social_analysis

cd /var/www/ai_social_analysis

cd ai_social_analysis
rm -f settings.py
ln -s settings-prod.py settings.py
cd -

python3 manage.py migrate
python3 manage.py collectstatic --noinput
python3 manage.py update_algorithms

service apache2 restart

