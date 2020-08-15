#include "fxlang/ShaderInfo.h"

#include <glslang/Include/intermediate.h>

namespace
{

std::string get_function_name(const std::string& functionSignature) noexcept 
{
  auto indexParenthesis = functionSignature.find("(");
  return functionSignature.substr(0, indexParenthesis);
}

// Traverse the AST to find the definition of a function based on its name/signature.
// e.g: prepareMaterial(struct-MaterialInputs-vf4-vf41;
class FunctionDefinitionFinder : public glslang::TIntermTraverser 
{
public:
    explicit FunctionDefinitionFinder(const std::string& functionName, bool useFQN = true)
            : mFunctionName (functionName), mUseFQN(useFQN) {
    }

    bool visitAggregate(glslang::TVisit, glslang::TIntermAggregate* node) override {
        if (node->getOp() == glslang::EOpFunction) {
            bool match;
            if (mUseFQN) {
                match = std::string(node->getName().c_str()) == mFunctionName.c_str();
            } else {
                std::string prospectFunctionName = get_function_name(node->getName().c_str());
                std::string cleanedFunctionName = get_function_name(mFunctionName);
                match = prospectFunctionName == cleanedFunctionName;
            }
            if (match) {
                mFunctionDefinitionNode = node;
                return false;
            }
        }
        return true;
    }

    glslang::TIntermAggregate* getFunctionDefinitionNode() const noexcept {
        return mFunctionDefinitionNode;
    }
private:
    const std::string& mFunctionName;
    bool mUseFQN;
    glslang::TIntermAggregate* mFunctionDefinitionNode = nullptr;
};

fxlang::ShaderInfo::FunctionParameter::Qualifier 
glslang_qualifier_to_func_param(glslang::TStorageQualifier q)
{
    switch (q) 
    {
    case glslang::EvqIn: 
        return fxlang::ShaderInfo::FunctionParameter::Qualifier::IN;
    case glslang::EvqInOut: 
        return fxlang::ShaderInfo::FunctionParameter::Qualifier::INOUT;
    case glslang::EvqOut: 
        return fxlang::ShaderInfo::FunctionParameter::Qualifier::OUT;
    case glslang::EvqConstReadOnly: 
        return fxlang::ShaderInfo::FunctionParameter::Qualifier::CONST;
    default: 
        return fxlang::ShaderInfo::FunctionParameter::Qualifier::IN;
    }
}

}

namespace fxlang
{

glslang::TIntermAggregate* 
ShaderInfo::GetFunctionByName(const std::string& name, TIntermNode& root) noexcept
{
    FunctionDefinitionFinder functionDefinitionFinder(name, false);
    root.traverse(&functionDefinitionFinder);
    return functionDefinitionFinder.getFunctionDefinitionNode();
}

void ShaderInfo::GetFunctionParameters(glslang::TIntermAggregate* func, std::vector<FunctionParameter>& output) noexcept 
{
    if (func == nullptr) {
        return;
    }

    // Does it have a list of params
    // The second aggregate is the list of instructions, but the function may be empty
    if (func->getSequence().size() < 1) {
        return;
    }

    // A function aggregate has a sequence of two aggregate children:
    // Index 0 is a list of params (IntermSymbol).
    for(TIntermNode* parameterNode : func->getSequence().at(0)->getAsAggregate()->getSequence()) {
        glslang::TIntermSymbol* parameter = parameterNode->getAsSymbolNode();
        FunctionParameter p = {
                parameter->getName().c_str(),
                parameter->getType().getCompleteString().c_str(),
                glslang_qualifier_to_func_param(parameter->getType().getQualifier().storage)
        };
        output.push_back(p);
    }
}

}