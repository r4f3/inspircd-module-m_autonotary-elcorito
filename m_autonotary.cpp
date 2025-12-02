/*
 * InspIRCd -- Internet Relay Chat Daemon
 *
 * m_autonotary: Sistema exclusivo de certificación automática y manual
 * Desarrollado para ElCorito.org — by r4f3
 *
 */

#include "inspircd.h"
#include "modules/whois.h"

class NotaryExt : public StringExtItem
{
public:
    NotaryExt(Module* parent)
        : StringExtItem("notary", ExtensionType::USER, parent)
    {
    }
};

class ModuleAutoNotary 
    : public Module
    , public Whois::LineEventListener
{
private:
    NotaryExt notary;
    bool trustgateway;
    int minregistered;
    int minontime;

public:
    ModuleAutoNotary()
        : Module(VF_VENDOR, "Certification system for ElCorito.org by r4f3")
        , Whois::LineEventListener(this)
        , notary(this)
    {
    }

    void ReadConfig(ConfigStatus&) override
    {
        auto tag = ServerInstance->Config->ConfValue("autonotary");

        trustgateway = tag->getBool("trustgateway", true);
        minregistered = tag->getInt("minregistered", 7);
        minontime = tag->getInt("minontime", 5);
    }

    void OnUserConnect(LocalUser* user) override
    {
        // Auto-certify based on connection class
        if (trustgateway && user->GetClass() && user->GetClass()->name == "webirc")
        {
            SetNotary(user, "Automatic (WebIRC Gateway)");
            return;
        }
    }

    void SetNotary(User* user, const std::string& reason)
    {
        notary.Set(user, reason);
        ServerInstance->SNO->WriteToSnoMask('a', "AUTONOTARY: %s ha sido verificado (%s)", 
            user->nick.c_str(), reason.c_str());
    }

    ModResult OnWhoisLine(Whois::Context& whois, Numeric::Numeric& numeric) override
    {
        // After channels line, append verification info
        if (numeric.GetNumeric() != RPL_WHOISCHANNELS)
            return MOD_RES_PASSTHRU;

        const std::string* val = notary.Get(whois.GetTarget());
        if (!val)
            return MOD_RES_PASSTHRU;

        whois.SendLine(RPL_WHOISACTUALLY, "%s :\x033✔ Usuario verificado en ElCorito.org (%s)", 
            whois.GetTarget()->nick.c_str(), val->c_str());

        return MOD_RES_PASSTHRU;
    }

    Command* GetCommands() override;
};

/*** COMMANDS ***/

class CommandNotary : public Command
{
private:
    NotaryExt& notary;

public:
    CommandNotary(Module* parent, NotaryExt& ext)
        : Command(parent, "NOTARY", 1)
        , notary(ext)
    {
        access_needed = CmdAccess::OPERATOR;
        syntax = "<ADD|DEL|INFO> <nick>";
    }

    CmdResult Handle(User* user, const Params& parameters) override
    {
        if (parameters.size() < 2)
            return CmdResult::FAILURE;

        const std::string& sub = parameters[0];
        const std::string& nick = parameters[1];

        User* target = ServerInstance->Users->Find(nick);
        if (!target)
        {
            user->WriteNotice("Usuario no encontrado.");
            return CmdResult::FAILURE;
        }

        if (sub == "ADD")
        {
            notary.Set(target, "Manual — Approved by " + user->nick);
            user->WriteNotice("Usuario verificado manualmente.");
            return CmdResult::SUCCESS;
        }
        else if (sub == "DEL")
        {
            notary.Unset(target);
            user->WriteNotice("Certificación eliminada.");
            return CmdResult::SUCCESS;
        }
        else if (sub == "INFO")
        {
            const std::string* val = notary.Get(target);
            if (val)
                user->WriteNotice("Estado: " + *val);
            else
                user->WriteNotice("No certificado.");
            return CmdResult::SUCCESS;
        }

        return CmdResult::FAILURE;
    }
};

/*** REGISTER COMMAND ***/

Command* ModuleAutoNotary::GetCommands()
{
    return new CommandNotary(this, notary);
}

MODULE_INIT(ModuleAutoNotary)
