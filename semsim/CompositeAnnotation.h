# ifndef SEMSIM_COMPOSITE_ANNOTATION_H_
# define SEMSIM_COMPOSITE_ANNOTATION_H_

# include "semsim/Preproc.h"
# include "semsim/AnnotationBase.h"
# include "semsim/PhysicalProperty.h"
# include "semsim/Entity.h"
# include "semsim/SingularAnnotation.h"

namespace semsim {
    /**
     * Composite annotations allow more precise descriptions of model
     * elements than singular ("plain") @ref Annotation "Annotations".
     * Whereas singular annotations specify the *chemical identity* of an SBML
     * species or the *physiological identity* of a CellML variable,
     * composite annotations also provide two additional pieces of information:
     * the @ref PhysicalProperty of an element, which specifies the physical
     * quantity the element represents (e.g. *concentration* or *amount* of an
     * SBML species; *liquid volume* or *electric current* of a CellML variable),
     * and a linear chain of terms (the @ref EntityDescriptor) that describes
     * *what* the element is (e.g. the chemical identity of an SBML species)
     * and *where* it is in the physical world.
     *
     * For example, to describe an SBML species representing the
     * *cytosolic glucose concentration in a pancreatic beta cell*,
     * one would need to specify that the species represents
     * glucose (<a href="https://identifiers.org/CHEBI:17234">CHEBI:17234</a>)
     * inside the cytosolic compartment (<a href="https://identifiers.org/GO:0005829">GO:0005829</a>)
     * of a pancreatic beta cell (<a href="https://identifiers.org/CL:0000169">CL:0000169</a>).
     * The following code snippet illustrates how to construct a @ref CompositeAnnotation
     * for this example:
     *
     * @code{.cpp}
     * // this example assumes you are using C++11 mode
     * // (controlled by the SEMSIM_CXX_STANDARD CMake variable)
     * @endcode
     */
    class SEMSIM_PUBLIC CompositeAnnotation : public AnnotationBase {
      public:
        /**
         * Construct a @ref CompositeAnnotation given a physical property (what is the quantity being represented - chemical concentration, fluid volume, etc.?)
         * and a domain descriptor ("what" is the chemical identity and "where" does the entity reside in physical space?)
         * @param property The physical property of this composite annotation. Always provided.
         * @param entity The entity contains all information outside of the physical property. This tells you "what" the element is (e.g. by specifying chemical identity) and "where". For example, if the annotation describes "cytosolic glucose concentration in a pancreatic beta cell", the entity would contain a definition (glycose) and two *structural relations* specifying the entity is *occurs in* the cytosol, which in turn *is part of* a pancreatic beta cell.
         */
        CompositeAnnotation(const std::string& metaid, const PhysicalProperty& property, const Entity& entity)
          : metaid_(metaid), property_(property), entity_(entity) {}

        # if __cplusplus >= 201103L
        /**
         * Construct a @ref CompositeAnnotation given a physical property (what is the quantity being represented - chemical concentration, fluid volume, etc.?)
         * and a domain descriptor ("what" is the chemical identity and "where" does the entity reside in physical space?)
         * @param property The physical property of this composite annotation. Always provided.
         * @param entity The entity contains all information outside of the physical property. This tells you "what" the element is (e.g. by specifying chemical identity) and "where". For example, if the annotation describes "cytosolic glucose concentration in a pancreatic beta cell", the entity would contain a definition (glycose) and two *structural relations* specifying the entity is *occurs in* the cytosol, which in turn *is part of* a pancreatic beta cell.
         */
        CompositeAnnotation(const std::string& metaid, PhysicalProperty&& property, Entity&& entity)
          : metaid_(metaid), property_(std::move(property)), entity_(std::move(entity)) {}
        # endif

        /// Copy constructor
        CompositeAnnotation(const CompositeAnnotation& other)
          : metaid_(other.metaid_),  property_(other.property_), entity_(other.entity_) {}

        # if __cplusplus >= 201103L
        /// Move constructor
        CompositeAnnotation(CompositeAnnotation&& other)
          : metaid_(std::move(other.metaid_)), property_(std::move(other.property_)), entity_(std::move(other.entity_)) {}
        # endif

        /**
         * Construct from a singular annotation.
         * Copy all definitions and terms.
         * @param other The singular annotation to copy.
         * @param property The physical property to assign to the composite annotation.
         * @param
         */
        CompositeAnnotation(const SingularAnnotation& other, const PhysicalProperty& property)
          : metaid_(other.getMetaId()), property_(property), entity_(other) {
            entity_.setMetaId(metaid_);
          }

        /**
         * This function returns @p true if the physical entity
         * descriptor is empty. This should not be the case
         * for any useful annotation.
         * @return Whether the physical entity descriptor is empty.
         */
        // bool isEntityEmpty() const {
          // return entity_.isEmpty();
        // }

        /**
         * @return The @ref EntityDescriptor describing the physical entity of this annotation.
         */
        // const EntityDescriptor& getEntity() const {
        //   return entity_;
        // }

        /**
         * This function returns @p true if the @ref Entity element of this composite annotation is empty.
         * The entity describes "what" the model element is and "where"
         * the it is located.
         * IGNORE:
         * It is typically empty for
         * annotations read in from SBML models, since SBML has
         * no way of expressing the "where" part of a composite annotation.
         * However, if the SBML entity is a **species** that resides in
         * a **compartment**, SemSim may automatically insert a domain
         * descriptor encoding the species/compartment relationship (
         * and this function will return false).
         * @return Whether the physical domain descriptor is empty.
         */
        bool isEntityEmpty() const {
          return entity_.isEmpty();
        }

        /**
         * @return The @ref EntityDescriptor describing the physical entity of this annotation.
         */
        const Entity& getEntity() const {
          return entity_;
        }

        /**
         * @return The @ref EntityDescriptor describing the physical entity of this annotation.
         */
        Entity& getEntity() {
          return entity_;
        }

        /**
         * Shortcut for adding a descriptor term to the last entity descriptor
         * (often, there is just one entity descriptor).
         * Create a new @ref EntityDescriptor if none exist.
         * @param relation The relation that the new term should use.
         * @param resource The resource that the term should point to.
         */
        void addTerm(const Relation& relation, const Resource& resource) {
          if(getEntity().getNumDescriptors() < 1)
            getEntity().addDescriptor(EntityDescriptor());
          getEntity().getDescriptors().back().addTerm(relation, resource);
        }

        /**
         * This function returns @p true if this annotation
           * can be encoded in an SBML model (i.e. its domain
         * descriptor must be empty).
         * The only exception to this is when the domain descriptor
         * consists of a single term, and that term describes the
         * *compartment* that the entity resides in.
         * @return [description]
         */
        // bool isSBMLCompatible() const {
        //   return isDomainEmpty();
        // }


        /// Create a copy of this object using the correct derived class's type.
        virtual AnnotationBase* clone() const {
          return new CompositeAnnotation(*this);
        }

        /**
         * Serialize this annotation to RDF using the Raptor library.
         * The RDF serialization format is chosen when initializing the
         * @c raptor_serializer, and must be done before calling this function.
         * @param world      Raptor world object. Must be initialized prior to calling this function.
         * @param serializer Raptor serializer object. Must be initialized prior to calling this function.
         */
        virtual void serializeToRDF(const URI& sbml_base_uri, raptor_world* world, raptor_serializer* serializer) const {
          entity_.serializeToRDF(sbml_base_uri, world, serializer);
          serializePhysicalPropertyToRDF(sbml_base_uri, world, serializer);
        }

        virtual std::string getRDF(const URI& sbml_base_uri, const std::string& format="rdfxml") const {
          raptor_world* world = raptor_new_world();
          raptor_serializer* serializer = raptor_new_serializer(world, format.c_str());
          if (!serializer)
            throw std::runtime_error("Could not create Raptor serializer for format "+format);

          raptor_uri* base_uri = raptor_new_uri(world, (const unsigned char*)"");

          raptor_serializer_set_namespace(serializer, raptor_new_uri(world, (const unsigned char*)bqb::root.c_str()), (const unsigned char*)"bqb");
          raptor_serializer_set_namespace(serializer, raptor_new_uri(world, (const unsigned char*)semsim::root.c_str()), (const unsigned char*)"semsim");

          void* output;
          size_t length;
          raptor_serializer_start_to_string(serializer, base_uri, &output, &length);

          serializeToRDF(sbml_base_uri, world, serializer);

          raptor_serializer_serialize_end(serializer);

          raptor_free_serializer(serializer);
          raptor_free_world(world);

          std::string result((char*)output);
          free(output);
          return result;
        }

        /**
         * @return the URI for this element (usually a local identifier).
         */
        const std::string& getMetaId() const {
          return metaid_;
        }

        /**
         * Set the meta id for this annotation.
         * @param metaid The meta id.
         */
        void setMetaId(const std::string& metaid) {
          metaid_ = metaid;
        }

        /**
         * Convert singular annotations to composite annotations
         * by copying their definitions and terms.
         * Effect when called on an instance of @ref CompositeAnnotation
         * is to create a clone.
         * @return A new composite annotation
         */
        AnnotationPtr makeComposite(const PhysicalProperty& prop) const {
          return AnnotationPtr(clone());
        }

        /**
         * Return a human--readable representation of the annotation
         * information. Ontology terms will be replaced with human-readable
         * names.
         */
        std::string humanize() const {
          return property_.humanize() + " -> (isPropertyOf) -> " + "#" + metaid_ + entity_.humanize();
        }

        virtual bool isComposite() const {
          return true;
        }

      protected:
        virtual void serializePhysicalPropertyToRDF(const URI& sbml_base_uri, raptor_world* world, raptor_serializer* serializer) const {
          const URI& phys_prop_uri = sbml_base_uri.withFrag(metaid_+"_property");
          const URI& phys_prop_def = property_.getResource().getURI();
          URI entity_uri = entity_.getURI(sbml_base_uri);

          // serialize physical property definition
          SerializeURIStatement(phys_prop_uri.encode(), bqb::isVersionOf.getURI().encode(), phys_prop_def.encode(), world, serializer);

          // serialize physical property to entity linkage
          SerializeURIStatement(phys_prop_uri.encode(), bqb::isPropertyOf.getURI().encode(), entity_uri.encode(), world, serializer);
        }

        /// Stores the URI of this element (usu. a local identifier)
        std::string metaid_;
        /// Stores the physical property for this annotation
        PhysicalProperty property_;
        /// Stores the physical domain descriptor for this annotation
        Entity entity_;
    };
}

# endif
