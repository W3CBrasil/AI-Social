class Box
  include Mongoid::Document
  include Mongoid::Attributes::Dynamic
  include Mongoid::Timestamps

  validates_presence_of :name
  field :category, type: String
  belongs_to :dataflow

  field :name, type: String
  field :category, type: String
  field :display_name, type: String
  field :parameters, type: BSON::Document
  field :datasets, type: BSON::Document, default: {}

  ALGORITHMS = {
    filter_elastic: 2,
    sentiment_analysis_elastic: 3,
    correlation_patern_elastic: 4,
    endorsement_elastic: 5,
    topics_elastic: 9,
  }

  def get_dataset_id
    dataset = parameters[:select][:Datasets][:selected]
    datasets[dataset]
  end

  def get_dataset_name
    dataset = parameters[:select][:Datasets][:selected]
    dataset
  end

  def get_algorithm_id algorithms
    if algorithms
      algorithms.each do |k,el|
        if el[:name] == "Filtragem"
          ALGORITHMS[:filter_elastic] = el[:id]
        elsif el[:name] == "Endosso"
          ALGORITHMS[:endorsement_elastic] = el[:id]
        elsif el[:name] == "Padrões de Correlação"
          ALGORITHMS[:correlation_patern_elastic] = el[:id]
        elsif el[:name] == "Análise de Sentimentos"
          ALGORITHMS[:sentiment_analysis_elastic] = el[:id]
        elsif el[:name] == "Tópicos"
          ALGORITHMS[:topics_elastic] = el[:id]
        end
      end
    end

    return ALGORITHMS[name.to_sym]
  end

  def is_input?
    display_name == 'Datasets'
  end

  def is_output?
    display_name == 'Output'
  end

  def is_algorithm?
    !(is_output? || is_input?)
  end

  def elastic_input dataset_input, algorithms
    dataset_input.each do |k|
      k.each do |v|
        if v['name']
          datasets[v['name']] = v['id']
          puts v
        end
      end
    end
    parameters['select']['Datasets']['list'] = dataset_input.map{|k,v| v['name']}
#    self.save
  end
end
