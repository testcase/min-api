/// @file
///	@ingroup 	minapi
///	@copyright	Copyright 2018 The Min-API Authors. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

namespace c74::min {

    #define MIN_AUTHOR static constexpr const char* class_author

    template<typename min_class_type>
    struct has_class_author {
        template<class, class>
        class checker;

        template<typename C>
        static std::true_type test(checker<C, decltype(&C::class_author)>*);

        template<typename C>
        static std::false_type test(...);

        typedef decltype(test<min_class_type>(nullptr)) type;
        static const bool value = is_same<std::true_type, decltype(test<min_class_type>(nullptr))>::value;
    };

    template<class min_class_type>
    typename enable_if<has_class_author<min_class_type>::value>::type doc_get_author(std::string& returned_class_author) {
        returned_class_author = min_class_type::class_author;
    }

    template<class min_class_type>
    typename enable_if<!has_class_author<min_class_type>::value>::type doc_get_author(std::string& returned_class_author) {
        returned_class_author = "Anonymous";
    }

    
    #define MIN_RELATED static constexpr const char* class_related

    template<typename min_class_type>
    struct has_class_related {
        template<class, class>
        class checker;

        template<typename C>
        static std::true_type test(checker<C, decltype(&C::class_related)>*);

        template<typename C>
        static std::false_type test(...);

        typedef decltype(test<min_class_type>(nullptr)) type;
        static const bool value = is_same<std::true_type, decltype(test<min_class_type>(nullptr))>::value;
    };

    template<class min_class_type>
    typename enable_if<has_class_related<min_class_type>::value>::type doc_get_related(strings& returned_tags) {
        returned_tags = str::split(min_class_type::class_related, ',');
    }

    template<class min_class_type>
    typename enable_if<!has_class_related<min_class_type>::value>::type doc_get_related(strings& returned_tags) {
        returned_tags = {};
    }


    #define MIN_DESCRIPTION static constexpr const char* class_description

    template<typename min_class_type>
    struct has_class_description {
        template<class, class>
        class checker;

        template<typename C>
        static std::true_type test(checker<C, decltype(&C::class_description)>*);

        template<typename C>
        static std::false_type test(...);

        typedef decltype(test<min_class_type>(nullptr)) type;
        static const bool value = is_same<std::true_type, decltype(test<min_class_type>(nullptr))>::value;
    };


    inline std::string doc_format(const std::string& input) {
        strings tokens = str::split(input, ' ');

        for (auto& token : tokens) {
            if (token[0] == '@') {    // attribute
                token = string("<at>") + token.substr(1, string::npos) + string("</at>");
            }
            else if (token[0] == '#') {    // message
                token = string("<m>") + token.substr(1, string::npos) + string("</m>");
            }
            else if (token[0] == '[' && token[token.size() - 1] == ']') {    // object
                token = string("<o>") + token.substr(1, token.size() - 2) + string("</o>");
            }
            else if (token == "&") {
                token = string("&amp;");
            }
        }

        return str::join(tokens);
    }

    template<class min_class_type>
    typename enable_if<has_class_description<min_class_type>::value>::type doc_get_description(std::string& returned_description) {
        returned_description = doc_format(min_class_type::class_description);
    }

    template<class min_class_type>
    typename enable_if<!has_class_description<min_class_type>::value>::type doc_get_description(std::string& returned_description) {
        returned_description = "Unknown";
    }

    #define MIN_DISCUSSION static constexpr const char* class_discussion

    template<typename min_class_type>
    struct has_class_discussion {
        template<class, class>
        class checker;

        template<typename C>
        static std::true_type test(checker<C, decltype(&C::class_discussion)>*);

        template<typename C>
        static std::false_type test(...);

        typedef decltype(test<min_class_type>(nullptr)) type;
        static const bool value = is_same<std::true_type, decltype(test<min_class_type>(nullptr))>::value;
    };

    template<class min_class_type>
    typename enable_if<has_class_discussion<min_class_type>::value>::type doc_get_discussion(std::string& returned_class_discussion) {
        returned_class_discussion = min_class_type::class_discussion;
    }
    
    template<class min_class_type>
    typename enable_if<!has_class_discussion<min_class_type>::value>::type doc_get_discussion(std::string& returned_class_discussion) {
        returned_class_discussion = "";
    }



    template<class min_class_type>
    void doc_generate(const min_class_type& instance, const std::string& refpage_fullpath, const std::string& max_class_name, const std::string& min_class_name) {
        documentation_flags flags = documentation_flags::none;

        class_get_flags<min_class_type>(instance, flags);
        if (flags == documentation_flags::do_not_generate)
            return;

        using namespace std;
        using std::endl;
        cout << "Updating Reference Page for class " << max_class_name << endl;

        ofstream refpage_file{refpage_fullpath};
        if (!refpage_file)
            error("failed to open file stream for refpage");

        refpage_file << "<?xml version='1.0' encoding='utf-8' standalone='yes'?>" << endl;
        refpage_file << endl << endl << "<!-- DO NOT EDIT THIS FILE ... YOU WILL LOSE YOUR WORK -->" << endl << endl;
        refpage_file << endl;

        // header

        refpage_file << "<c74object name='" << max_class_name << "' category='";

        strings class_tags;
        get_tags<min_class_type>(class_tags);
        for (auto i = 0; i < class_tags.size(); ++i) {
            if (i > 0)
                refpage_file << ", ";
            refpage_file << class_tags[i];
        }
        refpage_file << "'>" << endl;
        refpage_file << endl;

        // desc

        const size_t digest_length_max = 256;
        char         digest[digest_length_max];
        string       class_description;
        
        doc_get_description<min_class_type>(class_description);
        strncpy(digest, class_description.c_str(), digest_length_max);

        char* c = strstr(digest, ". ");
        if (!c)
            c = strchr(digest, '.');
        if (c)
            *c = 0;

        refpage_file << "	<digest>" << digest << "</digest>" << endl;
        refpage_file << "	<description>" << class_description << "</description>" << endl;
        refpage_file << endl << endl;

        // tags

        string author;
        doc_get_author<min_class_type>(author);

        refpage_file << "	<!--METADATA-->" << endl << endl;
        refpage_file << "	<metadatalist>" << endl;
        refpage_file << "		<metadata name='author'>" << doc_format(author) << "</metadata>" << endl;
        for (auto i = 0; i < class_tags.size(); ++i)
            refpage_file << "		<metadata name='tag'>" << str::trim(class_tags[i]) << "</metadata>" << endl;
        refpage_file << "	</metadatalist>" << endl;
        refpage_file << endl << endl;

        // inlets
        
        const auto& inlets = instance.inlets();
        if (!inlets.empty()) {
            refpage_file << "    <!--INLETS-->" << endl;
            refpage_file << "    <inletlist>" << endl;
            for(auto i=0;i<inlets.size();i++) { //need to get i for inlet id
                const string description = inlets[i]->description();
                const string attr_type   = inlets[i]->type();
                
                refpage_file << "        <inlet id='" << i << "' type='" << attr_type << "'>" << endl;
                refpage_file << "           <description>" << endl;
                refpage_file << "               " << description << endl;
                refpage_file << "           </description>" << endl;
                refpage_file << "        </inlet>" << endl;
            }
            refpage_file << "    </inletlist>" << endl;
            
        }
        
        // outlet
        
        const auto& outlets = instance.outlets();
        if (!outlets.empty()) {
            refpage_file << "    <!--OUTLETS-->" << endl;
            refpage_file << "    <outletlist>" << endl;
            for(auto i=0;i<outlets.size();i++) { //need to get i for inlet id
                const string description = outlets[i]->description();
                const string attr_type   = outlets[i]->type();
                
                refpage_file << "        <outlets id='" << i << "' type='" << attr_type << "'>" << endl;
                refpage_file << "           <description>" << endl;
                refpage_file << "               " << description << endl;
                refpage_file << "           </description>" << endl;
                refpage_file << "        </outlets>" << endl;
            }
            refpage_file << "    </outletlist>" << endl;
            
        }
        
        // arguments

        refpage_file << "	<!--ARGUMENTS-->" << endl << endl;
        refpage_file << "	<objarglist>" << endl << endl;

        const auto& arguments = instance.arguments();

        for (const auto& arg : arguments) {
            const auto& description = doc_format(arg->description_string());
            const auto& type        = arg->type();
            bool        required    = arg->required();

            strncpy(digest, description.c_str(), digest_length_max);
            char* c = strstr(digest, ". ");
            if (!c)
                c = strchr(digest, '.');
            if (c)
                *c = 0;

            refpage_file << "		<objarg name='" << arg->name() << "'";
            if (required)
                refpage_file << " optional='0'";
            else
                refpage_file << " optional='1'";
            if (!type.empty())
                refpage_file << " type='" << type << "'";
            refpage_file << ">" << endl;
            refpage_file << "			<digest>" << digest << "</digest>" << endl;
            refpage_file << "			<description>" << description << "</description>" << endl;
            refpage_file << "		</objarg>" << endl << endl;
        }

        refpage_file << "	</objarglist>" << endl;
        refpage_file << endl << endl;

        // messages

        refpage_file << "	<!--MESSAGES-->" << endl << endl;
        refpage_file << "	<methodlist>" << endl << endl;

        const auto& messages = instance.messages();

        for (const auto& p : messages) {
            const auto& message_object = *p.second;
            if (message_object.type() != max::A_CANT) {
                const auto& description = doc_format(message_object.description_string());

                strncpy(digest, description.c_str(), digest_length_max);
                char* c = strstr(digest, ". ");
                if (!c)
                    c = strchr(digest, '.');
                if (c)
                    *c = 0;

                refpage_file << "		<method name='" << message_object.name() << "'>" << endl;
                refpage_file << "			<digest>" << digest << "</digest>" << endl;
                refpage_file << "			<description>" << description << "</description>" << endl;
                refpage_file << "		</method>" << endl << endl;
            }
        }

        refpage_file << "	</methodlist>" << endl;
        refpage_file << endl << endl;

        // attributes

        refpage_file << "	<!--ATTRIBUTES-->" << endl << endl;
        refpage_file << "	<attributelist>" << endl << endl;

        const auto& attributes = instance.attributes();

        for (const auto& p : attributes) {
            try {
                const auto& attr_object = *p.second;
                const auto& description = doc_format(attr_object.description_string());
                const auto& attr_type   = attr_object.datatype();

                if (attr_object.visible() == visibility::hide)
                    continue;

                strncpy(digest, description.c_str(), digest_length_max);
                char* c = strstr(digest, ". ");
                if (!c)
                    c = strchr(digest, '.');
                if (c)
                    *c = 0;
          
                if(attr_object.size() > 1) {
                    refpage_file << "        <attribute name='" << attr_object.name() << "' get='1' set='" << attr_object.writable() << "' type='" << attr_type << "' size='" << attr_object.size() << "'" << ">" << endl;;
                } else {
                    refpage_file << "        <attribute name='" << attr_object.name() << "' get='1' set='" << attr_object.writable() << "' type='" << attr_type << "' size='1'" << ">" << endl;;
                }
                refpage_file << "			<digest>" << digest << "</digest>" << endl;
                
                refpage_file << "            <description>" << description;
                
                if(attr_object.default_string() != "") {
                    refpage_file << " (default = " << attr_object.default_string() << ")" ;
                }
                refpage_file << "           </description>" << endl;
                
                if((attr_type == "symbol" ) && (!attr_object.range_string().empty())) {
                    vector <string> tokens;
                    istringstream iss(attr_object.range_string());
                    while(iss) {
                        string word;
                        iss >> word;
                        if(word != "") {
                            tokens.push_back(word);
                        }
                    }
                    
                    refpage_file << "           <attributelist>" << endl;
                    refpage_file << "               <attribute name='enumvals' get='1' set='1' type='atom' size='" << tokens.size() <<"'" << ">" << endl;
                    refpage_file << "                   <enumlist>" << endl;
                   
                    
                    for(auto i=0;i<tokens.size();i++) {
                        refpage_file << "                       <enum name=" << tokens[i] << ">" << endl ;
                        refpage_file << "                       </enum>" << endl ;
                    }
                    
                    refpage_file << "                   </enumlist>" << endl;
                    refpage_file << "               </attribute>" << endl;
                    refpage_file << "               <attribute name='label' get='1' set='1' type='symbol' size='1' value='" << attr_object.label_string() << "' />" << endl;
                    refpage_file << "               <attribute name='style' get='1' set='1' type='symbol' size='1' value='enum' />" << endl;
                    refpage_file << "           </attributelist>" << endl;
                    
                    refpage_file << endl;
                }
               
                refpage_file << "		</attribute>" << endl << endl;
            }
            catch (...) {
                // if an attr doesn't have a description or there is some other problem, just ignore this attribute
            }
        }

        refpage_file << "	</attributelist>" << endl;
        refpage_file << endl << endl;

        // related (see also)

        refpage_file << "	<!--RELATED-->" << endl << endl;
        refpage_file << "	<seealsolist>" << endl;
        strings related;
        doc_get_related<min_class_type>(related);
        for (auto i = 0; i < related.size(); ++i)
            refpage_file << "		<seealso name='" << str::trim(related[i]) << "' />" << endl;
        refpage_file << "	</seealsolist>" << endl;
        refpage_file << endl << endl;

       
        
        // discussion
        refpage_file << "    <!--DISCUSSION-->" << endl << endl;

        string class_discussion;
        doc_get_discussion<min_class_type>(class_discussion);
        if(class_discussion != "") {
            refpage_file << "    <discussion>" << class_discussion << "</discussion>" << endl;
        }
        
        
         // footer
        refpage_file << "</c74object>" << endl;
    }


    template<class min_class_type>
    void doc_update(const min_class_type& instance, const std::string& max_class_name, const std::string& min_class_name) {
        try {
            path        extern_file(max_class_name, path::filetype::external);
            auto        extern_date     = extern_file.date_modified();
            std::string extern_fullpath = extern_file;

            std::string docs_fullpath = extern_fullpath;
            docs_fullpath.erase(docs_fullpath.rfind('/'));
            docs_fullpath.erase(docs_fullpath.rfind('/'));
            docs_fullpath += "/docs";

            path        docs_folder(docs_fullpath, path::filetype::folder, true);
            std::string refpage_fullpath = docs_folder;
            refpage_fullpath += "/" + max_class_name + ".maxref.xml";
            path           refpage_file;
            path::filedate refpage_date{};

            try {
                refpage_file = path(refpage_fullpath);
                refpage_date = refpage_file.date_modified();
            }
            catch (std::exception&) {
            }

            if (!refpage_file || refpage_date < extern_date) {
                doc_generate(instance, refpage_fullpath, max_class_name, min_class_name);
            }
        }
        catch (std::exception& e) {
            std::cerr << "DOC UPDATE ERROR: " << e.what() << std::endl;
        }
    }

}    // namespace c74::min
