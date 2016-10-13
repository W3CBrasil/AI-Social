class Template
  include Mongoid::Document
  include Mongoid::Attributes::Dynamic
  include Mongoid::Timestamps

  validates_presence_of :name
  validates_uniqueness_of :name

  field :name, type: String
  field :display_name, type: String
  field :category, type: String
  field :parameters, type: BSON::Document
end
