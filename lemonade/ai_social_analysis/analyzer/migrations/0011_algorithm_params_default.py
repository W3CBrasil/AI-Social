# -*- coding: utf-8 -*-
# Generated by Django 1.9.2 on 2016-03-03 21:17
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('analyzer', '0010_auto_20160303_1811'),
    ]

    operations = [
        migrations.AddField(
            model_name='algorithm',
            name='params_default',
            field=models.TextField(default=''),
            preserve_default=False,
        ),
    ]