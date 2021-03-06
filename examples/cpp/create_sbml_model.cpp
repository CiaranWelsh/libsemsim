# include <semsim/SemSim.h>

# include "sbml/SBMLTypes.h"

# include <iostream>

using namespace semsim;

int main() {
  // create SBML model
  LIBSBML_CPP_NAMESPACE_QUALIFIER SBMLDocument* d =
    new LIBSBML_CPP_NAMESPACE_QUALIFIER SBMLDocument(3, 2);
  LIBSBML_CPP_NAMESPACE_QUALIFIER Model* m = d->createModel();
  m->setId("beta_cell_model");

  // create a compartment to represent the cytosol
  LIBSBML_CPP_NAMESPACE_QUALIFIER Compartment* comp = m->createCompartment();
  comp->setId("cytosol");
  comp->setMetaId("cytosol");
  comp->setSize(1);
  comp->setConstant(true);

  // add a CV term for the cytosol
  LIBSBML_CPP_NAMESPACE_QUALIFIER CVTerm *cv_cytosol = new LIBSBML_CPP_NAMESPACE_QUALIFIER CVTerm();
  cv_cytosol->setQualifierType(LIBSBML_CPP_NAMESPACE_QUALIFIER BIOLOGICAL_QUALIFIER);
  cv_cytosol->setBiologicalQualifierType(LIBSBML_CPP_NAMESPACE_QUALIFIER BQB_IS);
  cv_cytosol->addResource(GO::get(5829).getURI().encode());
  comp->addCVTerm(cv_cytosol);

  LIBSBML_CPP_NAMESPACE_QUALIFIER UnitDefinition* unitdef = m->createUnitDefinition();
  unitdef->setId("molar");

  LIBSBML_CPP_NAMESPACE_QUALIFIER Unit* unit = unitdef->createUnit();
  unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_MOLE);
  unit->setExponent(1);
  unit->setMultiplier(1);
  unit->setScale(1);

  unit = unitdef->createUnit();
  unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_LITRE);
  unit->setExponent(-1);
  unit->setMultiplier(1);
  unit->setScale(1);

  // create the species for glucose
  LIBSBML_CPP_NAMESPACE_QUALIFIER Species* s = m->createSpecies();
  s->setCompartment("cytosol");
  s->setId("glucose");
  s->setMetaId("glucose");
  s->setInitialConcentration(0);
  s->setUnits("molar");
  s->setHasOnlySubstanceUnits(false);
  s->setConstant(false);
  s->setBoundaryCondition(false);

  // add a CV term to glucose
  LIBSBML_CPP_NAMESPACE_QUALIFIER CVTerm *cv_glucose = new LIBSBML_CPP_NAMESPACE_QUALIFIER CVTerm();
  cv_glucose->setQualifierType(LIBSBML_CPP_NAMESPACE_QUALIFIER BIOLOGICAL_QUALIFIER);
  cv_glucose->setBiologicalQualifierType(LIBSBML_CPP_NAMESPACE_QUALIFIER BQB_IS);
  cv_glucose->addResource(CHEBI::get(17234).getURI().encode());
  s->addCVTerm(cv_glucose);

  // create import reaction for glucose
  LIBSBML_CPP_NAMESPACE_QUALIFIER Reaction* reaction = m->createReaction();
  reaction->setId("glucose_import");
  reaction->setReversible(false);

  LIBSBML_CPP_NAMESPACE_QUALIFIER SpeciesReference* sr = reaction->createProduct();
  sr->setSpecies("glucose");
  sr->setConstant(false);
  LIBSBML_CPP_NAMESPACE_QUALIFIER KineticLaw* k = reaction->createKineticLaw();
  LIBSBML_CPP_NAMESPACE_QUALIFIER Parameter* p = k->createParameter();
  p->setId("glucose_import_rate");
  p->setValue(1);
  k->setMath(LIBSBML_CPP_NAMESPACE_QUALIFIER SBML_parseL3FormulaWithModel("glucose_import_rate", m));
  // alternate way to set formula
  // LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode* a =
  //   new LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode(LIBSBML_CPP_NAMESPACE_QUALIFIER AST_NAME);
  // a->setName("glucose_import_rate");
  // k->setMath(a);

  LIBSBML_CPP_NAMESPACE_QUALIFIER SBMLWriter sbml_writer;

  std::cerr << "**********************************************************\n";
  std::cerr << "SBML output:\n";
  std::cerr << "**********************************************************\n\n";

  std::cerr << sbml_writer.writeSBMLToString(d) << "\n";

  // importing the model into libSemSim will automatically
  // add metaids for any annotatable SBML elements that lack them
  SBMLImporter importer(d);
  SBMLModel& model = importer.getSBMLModel();

  std::cerr << "**********************************************************\n";

  std::cerr << "RDF serialization of annotations:\n";
  std::cerr << "**********************************************************\n\n";

  std::cerr << model.getRDF("./mymodel.xml", "turtle") << "\n";

  // this isn't bad - the RDF already contains a composite annotation
  // telling is that the species "glucose" is inside the "cytosol"
  // we currently have "cytosolic glucose"
  // however - what we want is "cytosolic glucose in a pancreatic beta cell"
  // to describe this, we need to annotate the compartment (not the species!)
  // and describe the cell type it is part of

  // the compartment currently has a *singular annotation*, which can't
  // represent membership in a "pancreatic beta cell", so we change this to
  // a composite annotation
  Component* c = model.getComponentForId("cytosol");
  c->setAnnotation(c->getAnnotation().makeComposite(OPB::get(523))); // 523 is spatial volume
  // now add a descriptor term specifying that the cytoplasm
  // is part of a pancreatic beta cell (CL:0000169)
  c->getCompositeAnnotation().addTerm(
    bqb::isPartOf, // the relation (the cytoplasm *is part of* ...)
    CL::get(169)   // the resource (pancreatic beta cell)
  );

  std::cerr << "**********************************************************\n";
  std::cerr << "RDF serialization with cell type annotation:\n";
  std::cerr << "**********************************************************\n\n";

  std::cerr << model.getRDF("./mymodel.xml", "turtle") << "\n";

  std::cerr << "**********************************************************\n";
  std::cerr << "Humanized version of the annotation information\n";
  std::cerr << "(ontology terms replaced by human-readable names):\n";
  std::cerr << "**********************************************************\n\n";

  std::cerr << model.humanize() << "\n";

  exportToOmex(model, "/tmp/combine.omex");

  return 0;
}
