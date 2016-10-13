import datetime
import json
import elasticsearch
import elasticsearch.helpers

from django.core.management.base import BaseCommand

from django.contrib.auth.models import User, Group
from analyzer.models import Dataset


class Command(BaseCommand):

    help = 'Create a new project and populate the canonical dataset'

    def add_arguments(self, parser):

        parser.add_argument("project_name", 
            help="Name of the new project")

        parser.add_argument("data_filename", 
            help="Path of the file where the documents are stored")

        parser.add_argument("properties_filename", 
            help="Path of the file where the properties of the index is stored")

        parser.add_argument('--overwrite',
            action='store_true',
            dest='overwrite',
            default=False,
            help='Overwrite the elastic search index')


    def generate_actions(filename_input, index_name):

        with open(filename_input, "r", encoding="utf-8") as infile:

            for line in infile:

                obj = json.loads(line)
                obj["comment_date"] = datetime.datetime.strptime(obj["comment_date"], 
		                                                 "%Y-%m-%d %H:%M:%S") 

                action = {
                    "index": {
                        "_index": index_name, 
                        "_type": "comment", 
                        "_id": obj["comment_id"]
                    }
                }

                yield action
                yield obj

 
    def handle(self, *args, **options):

        if options["overwrite"]:
            if Group.objects.filter(name=options["project_name"]).exists():
                project = Group.objects.get(name=options["project_name"])
                project.delete()
        
	# Create project
        project = Group(name=options["project_name"])
        project.save()

        # Create dataset
        dataset = Dataset.create_new(project)

        try:
            es = elasticsearch.Elasticsearch()
            
            # Create index

            if options["overwrite"] and es.indices.exists(index=dataset.name):
                es.indices.delete(index=dataset.name)

            with open(options["properties_filename"], "r") as infile:
                index_properties = json.load(infile)
            
            es.indices.create(index=dataset.name, body=index_properties)

	    
            # Populate index

            with open(options["data_filename"], "r", encoding="utf-8") as infile:
                for line in infile:
                    obj = json.loads(line)
                    es.index(index=dataset.name, doc_type="comment", body=obj)

            #TODO: bulk
            #actions = Command.generate_actions(options["data_filename"], dataset.name)
            #elasticsearch.helpers.bulk(es, actions)
            
            # Update fields of dataset
            dataset.fill_es_fields()

            self.stdout.write(self.style.SUCCESS("Successfully created and populated project \"{}\"".format(options["project_name"])))

        except Exception as e:
            dataset.delete()
            project.delete()
            raise e

