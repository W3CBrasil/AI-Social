class Graphic
  include Mongoid::Document

  validates_presence_of :name
  validates_uniqueness_of :name

  field :name, type: String
  field :category, type: String
  field :position, type: BSON::Document
  field :size, type: BSON::Document
  field :outPorts, type: BSON::Document
  field :inPorts, type: BSON::Document
  field :attrs, type: BSON::Document
end
