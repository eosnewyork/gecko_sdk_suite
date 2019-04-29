## Make sure that our path has just top level of ddml
#@PydevCodeAnalysisIgnore
import sys
import os
import glob
import copy

# get module mapping
from efm32wg.modules.PIN.PIN_Class import *
import efm32wg.PythonSnippet.ExporterModel as ExporterModel
import efm32wg.PythonSnippet.Metadata as Metadata
import efm32wg.halconfig.halconfig_dependency as dep

from efm32wg.PythonSnippet.CodeGen import *
from efm32wg.PythonSnippet.StudioProject import *

import efm32wg.modules.BTL_BUTTON.BTL_BUTTON_behavior as BTL_BUTTON_behavior
import efm32wg.modules.BUTTON.BUTTON_behavior as BUTTON_behavior
import efm32wg.modules.CMU.CMU_behavior as CMU_behavior
import efm32wg.modules.EMU.EMU_behavior as EMU_behavior
import efm32wg.modules.EXTFLASH.EXTFLASH_behavior as EXTFLASH_behavior
import efm32wg.modules.GPIO.GPIO_behavior as GPIO_behavior
import efm32wg.modules.I2C0.I2C_behavior as I2C_behavior
import efm32wg.modules.I2C1.I2C_behavior as I2C_behavior
import efm32wg.modules.I2CSENSOR.I2CSENSOR_behavior as I2CSENSOR_behavior
import efm32wg.modules.IOEXP.IOEXP_behavior as IOEXP_behavior
import efm32wg.modules.LED.LED_behavior as LED_behavior
import efm32wg.modules.LEUART0.LEUART_behavior as LEUART_behavior
import efm32wg.modules.LEUART1.LEUART_behavior as LEUART_behavior
import efm32wg.modules.PRS.PRS_behavior as PRS_behavior
import efm32wg.modules.SERIAL.SERIAL_behavior as SERIAL_behavior
import efm32wg.modules.SPIDISPLAY.SPIDISPLAY_behavior as SPIDISPLAY_behavior
import efm32wg.modules.SPINCP.SPINCP_behavior as SPINCP_behavior
import efm32wg.modules.TIMER0.TIMER_behavior as TIMER_behavior
import efm32wg.modules.TIMER1.TIMER_behavior as TIMER_behavior
import efm32wg.modules.TIMER2.TIMER_behavior as TIMER_behavior
import efm32wg.modules.TIMER3.TIMER_behavior as TIMER_behavior
import efm32wg.modules.UART0.UART_behavior as UART_behavior
import efm32wg.modules.UART1.UART_behavior as UART_behavior
import efm32wg.modules.UARTNCP.UARTNCP_behavior as UARTNCP_behavior
import efm32wg.modules.USART0.USART_behavior as USART_behavior
import efm32wg.modules.USART1.USART_behavior as USART_behavior
import efm32wg.modules.USART2.USART_behavior as USART_behavior
import efm32wg.modules.VCOM.VCOM_behavior as VCOM_behavior
import efm32wg.modules.WDOG.WDOG_behavior as WDOG_behavior
def generate(context):
    """
    Generates the relevant defines for hwconf setup to hal-config/hal-config.h
    """
    root = initSession(context)

    # Get output directory from config or fall back to 'hal-config' directory
    output_dir = getVariable('output_dir')
    if not output_dir:
        output_dir = 'hal-config'
    # Get output filename from config or fall back to 'hal-config.h'
    output_file = getVariable('output_file')
    if not output_file:
        output_file = 'hal-config.h'
    hal_conf_dir = newDirectory(root, output_dir)
    hal_conf_h = newFile(hal_conf_dir, output_file)

    define_guard = output_file.upper().replace('-','_').replace('.','_')
    # Generate header and start of cloaking
    text = ""
    text += "#ifndef {}\n".format(define_guard)
    text += "#define {}\n".format(define_guard)
    text += "\n"
    text += "#include \"em_device.h\"\n"
    text += "#include \"hal-config-types.h\"\n"
    text += "\n"
    text += "// This file is auto-generated by Hardware Configurator in Simplicity Studio.\n"
    text += "// Any content between $[ and ]$ will be replaced whenever the file is regenerated.\n"
    text += "// Content outside these regions will be preserved.\n"

    # Push location for eval'd text
    pushContext(location=hal_conf_h)
    newContribution(hal_conf_h, text)

    # Initializing lists
    define_list = []
    region_list = []
    module_list = []

    # Generate regions in file
    for transition in modeTransitions():

        # Get region names from transition.modules
        for module in transition.modules:
            module_type = module.obj.object.getComponentId().split(".")[2]
            if not (module_type == "port" or module_type == "pin"):
                region_list.append(module.name)

        # Generate regions alphabetically in file
        region_list.sort()
        for region in region_list:
            if not getLocation(region):
                startRegion(region)
                endRegion()

        # Adding portio defines to define_list
        portio = transition.getModule("PORTIO").obj.object
        routes = add_portio_defines(portio, define_list)

        available_modules = Metadata.get_available_modules_for_family()
        familyobj = dep.Family('efm32wg')

        # Generate a module list from '_behaviour' files
        
        mod_inst = BTL_BUTTON_behavior.BTL_BUTTON('BTL_BUTTON')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = BUTTON_behavior.BUTTON('BUTTON')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = CMU_behavior.CMU('CMU')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = EMU_behavior.EMU('EMU')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = EXTFLASH_behavior.EXTFLASH('EXTFLASH')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = GPIO_behavior.GPIO('GPIO')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = I2C_behavior.I2C('I2C0')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = I2C_behavior.I2C('I2C1')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = I2CSENSOR_behavior.I2CSENSOR('I2CSENSOR')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = IOEXP_behavior.IOEXP('IOEXP')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = LED_behavior.LED('LED')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = LEUART_behavior.LEUART('LEUART0')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = LEUART_behavior.LEUART('LEUART1')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = PRS_behavior.PRS('PRS')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = SERIAL_behavior.SERIAL('SERIAL')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = SPIDISPLAY_behavior.SPIDISPLAY('SPIDISPLAY')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = SPINCP_behavior.SPINCP('SPINCP')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = TIMER_behavior.TIMER('TIMER0')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = TIMER_behavior.TIMER('TIMER1')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = TIMER_behavior.TIMER('TIMER2')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = TIMER_behavior.TIMER('TIMER3')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = UART_behavior.UART('UART0')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = UART_behavior.UART('UART1')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = UARTNCP_behavior.UARTNCP('UARTNCP')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = USART_behavior.USART('USART0')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = USART_behavior.USART('USART1')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = USART_behavior.USART('USART2')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = VCOM_behavior.VCOM('VCOM')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        
        mod_inst = WDOG_behavior.WDOG('WDOG')
        mod_inst.load_halconfig_model(available_modules, familyobj)
        module_list.append(mod_inst)
        

        # Adding peripheral defines to list
        add_peripheral_defines(module_list, transition, define_list, routes)

        # Printing the list to hal-config.h
        print_defines_from_list(define_list)


    # Add module include
    text = ""
    text += "#if defined(_SILICON_LABS_MODULE)\n"
    text += "#include \"sl_module.h\"\n"
    text += "#endif\n"
    text += "\n"
    newContribution(hal_conf_h, text)

    # Ending the #ifndef
    newContribution(hal_conf_h, "#endif /* {} */".format(define_guard))
    popContext()

    # Sync project modules
    try:
        framework = detect_framework(context)
        refresh_studio_modules(framework, module_list)
    except:
        print("project module sync failed")

def add_portio_defines(portio, define_list):
    """
    Adding defines for the active routes found in portio to define_list
    :type portio: Studio._private.StudioClasses.com_silabs_ide_halconf_core_client_accessors_IPortIOAccessor
    :param define_list: list of dictionaries containing the relevant info for #defines
    """

    routes = {}
    common_routes = []

    # Iterating through active routes, adding them to defineList
    for route in activePinRoutes(portio):
        mod_label = route.getLabel()
        pin_name = route.getPinName()
        location = route.location.getLocationNumber() if route.selector.locationPropertyReference else None
        mod_name = route.getModuleName()
        if len(route.selector.routes) > 1:
            # Multiple routes on this selector
            if mod_name == 'GPIO':
                # DBG -> DBGROUTE, ETM -> ETMROUTE, etc
                common_route = "{}ROUTE".format(route.selector.getName())
            elif route.selector.getName().endswith('LOC'):
                # ETH_RMIILOC -> RMII, etc
                common_route = route.selector.getName().split('_')[1][:-3]
            elif mod_name == 'PRS':
                common_route = 'PRSROUTE'
            else:
                common_route = 'ROUTE'
        else:
            common_route = None

        # If route is connected to the APORT generate a different location to match data sheet
        aport_location = None
        if route.getAportName():
            aport_name = route.getAportName()
            mod_label = mod_label.rsplit("_", 1)[0]
            aport_location = location
            location = generate_aport_loc_define(aport_name, mod_name, mod_label, location)

        routes[mod_label] = {
            "module": mod_name,
            "pin_name": pin_name,
            "common_route": common_route,
            "location": location,
        }

        if aport_location is not None:
            routes[mod_label]['aport_location'] = aport_location

        # Append define dict to define_list
        def_label = "{}_{}".format("PORTIO", mod_label)
        def_common_label = "{}_{}_{}".format("PORTIO", mod_name, common_route)

        define_list.append({"region": mod_name, "label": def_label, "pin_name": pin_name})
        if location is not None:
            if common_route:
                if def_common_label not in common_routes:
                    # Not seen this common route before
                    define_list.append({"region": mod_name, "label": def_common_label, "loc": location})
                    common_routes.append(def_common_label)
            else:
                define_list.append({"region": mod_name, "label": def_label, "loc": location})

        define_list.append({"region": mod_name, "label": "", "def_value": "newline"})

    # Looking at all pins, searching for custom pin names
    for port_bank in portio.getPortBanks():
        for pin in port_bank.getPins():

            # If pin has custom name generate #defines for this name also
            if pin.ref(PORT_PINS.ports_settings_custompinname.S).get():
                custom_pin_name = pin.ref(PORT_PINS.ports_settings_custompinname.S).get()
                if pin.ref(PORT_PINS.ports_settings_custompinname.S).isReadOnly():
                    continue
                def_label = custom_pin_name
                define_list.append({"region": "Custom pin names", "label": def_label, "pin_name": pin.getName()})
                define_list.append({"region": "Custom pin names", "label": "", "def_value": "newline"})

    return routes


def generate_peripheral(exp_module,
                        studio_module,
                        define_list,
                        routes,
                        mode,
                        def_region,
                        output_prefix,
                        custom_name = None,
                        prevent_override = False):

    peripheral_pins = []
    peripheral_props = []
    peripheral_custom_props = []
    inherited_props = []
    override_props = []
    inherit_prop_list = []

    # This peripheral has overridden configuration options (e.g. internal SPI flash)
    overrides = {}
    if hasattr(exp_module, "generate_define_override"):
        for prop in exp_module.generate_define_override(studio_module):
            overrides[prop['label']] = prop
            override_props.append(prop)

    common_routes = []

    show_advanced = studio_module.ref(exp_module.get_property('showadvanced').id).get() == '1'

    for prop in exp_module.get_properties():
        if prop.mode:
            # Make sure property exists in the active mode
            if isinstance(prop.mode, list):
                if mode not in prop.mode:
                    continue
            elif prop.mode != mode:
                    continue

        # Only create #defines from uppercase properties
        if not prop.name.isupper():
            continue

        # Only generate hidden properties if they should be generated when hidden
        if not prop.generate_if_hidden and studio_module.ref(prop.id).isHidden():
            continue

        # Only print advanced properties if we are in advanced mode
        if not show_advanced and prop.is_advanced:
            continue

        def_label = prop.define_name.replace(prop.parent.namespace, output_prefix)
        if custom_name:
            custom_label = prop.define_name.replace(prop.parent.namespace, custom_name).replace('HAL_','').replace('BSP_','')
        prop_value = studio_module.ref(prop.id).get()

        # Filter out disabled/none values
        if prop_value == "Disabled" or prop_value == "None":
            continue

        if def_label in overrides:
            # Don't generate native defines for properties that will be overridden
            continue

        if prevent_override:
            # Prevent this define generation from overriding an existing one
            already_defined = False
            for define in define_list:
                if define['region'] == def_region and define['label'] == def_label:
                    already_defined = True
            if already_defined:
                continue

        # If property is a pin
        if isinstance(prop, ExporterModel.PinProperty):
            if not prop.mode and studio_module.ref(prop.id).isHidden():
                # Don't generate defines for hidden pin properties unless they are in the active mode
                continue

            pin_name = prop_value
            pin_obj = {"region": def_region, "label": def_label, "pin_name": pin_name}
            common_route_obj = None

            # If prop has a referenced route to portio
            custom_route = None
            if prop.referenced_route:
                route = routes.get("{}_{}".format(prop.referenced_module, prop.referenced_route), None)
                if route:
                    if route['common_route']:
                        if route['common_route'] not in common_routes:
                            common_routes.append(route['common_route'])
                            def_label = "{}_{}".format(def_label.rsplit('_', 1)[0], route['common_route'])
                            common_route_obj = {"region": def_region, "label": def_label, "loc": route['location']}
                    else:
                        pin_obj['loc'] = route['location']
                else:
                    print("Error: referenced route, but no route found for " + prop.name)

            peripheral_pins.append(pin_obj)
            if common_route_obj:
                peripheral_pins.append(common_route_obj)

            # If pin has em4 attribute True
            if prop.em4:
                peripheral_pins.append({"region": def_region, "label": def_label + "_EM4WU", "def_value": Metadata.em4_pin_to_loc(pin_name)["define"]})

            # If peripheral has a non-empty custom name
            if custom_name:
                custom_pin_obj = copy.deepcopy(pin_obj)
                custom_pin_obj['label'] = custom_label

                peripheral_pins.append(custom_pin_obj)
                if common_route_obj:
                    custom_route_obj = copy.deepcopy(common_route_obj)
                    custom_route_obj['label'] = custom_route_obj['label'].replace(output_prefix, custom_name).replace('HAL_','').replace('BSP_','')
                    peripheral_pins.append(custom_route_obj)
                if prop.em4:
                    peripheral_pins.append({"region": def_region, "label": custom_label + "_EM4WU", "def_value": Metadata.em4_pin_to_loc(pin_name)["define"]})

            peripheral_pins.append({"region": def_region, "label": "", "def_value":"newline"})

        elif isinstance(prop, ExporterModel.AportBusProperty):
            def_value = prop_value
            for value in prop.values.values():
                if value.value == studio_module.ref(prop.id).get():
                    def_value = value.define_value

            if def_value.startswith("APORT"):
                route = routes.get("{}_{}".format(prop.parent.name, prop.signal), None)
                if not route:
                    # APORT selected but no channel output enabled. Skip this property
                    continue
                ch = "CH{}".format(route['aport_location'])
                def_value = def_value + ch

            # Handle prefix
            if hasattr(prop, "define_value_prefix"):
                def_value = prop.define_value_prefix + def_value

            # Putting the define strings into define_list
            peripheral_props.append({"region": def_region, "label": def_label, "def_value": str(def_value),
                                     "is_array": False, "is_str_prop": False})
            if custom_name:
                peripheral_custom_props.append({"region": def_region, "label": custom_label, "def_value": str(def_value),
                                         "is_array": False, "is_str_prop": False})
        else:
            def_value = prop_value

            if isinstance(prop, ExporterModel.IntegerProperty):
                if prop.format:
                    # Format integer
                    def_value = prop.format.format(def_value)
                else:
                    # Handle integer properties that should have literal suffixes
                    if prop.min >= 0:
                        def_value = str(def_value) + 'U'
                    if prop.max >= 65535:
                        def_value = str(def_value) + 'L'


            if isinstance(prop, ExporterModel.EnumProperty):
                for value in prop.values.values():
                    if value.value == studio_module.ref(prop.id).get():
                        def_value = value.define_value

            # Handle prefix
            if hasattr(prop, "define_value_prefix"):
                def_value = prop.define_value_prefix + str(def_value)

            # Handle array properties that should print {} rather than ()
            if isinstance(prop, ExporterModel.ArrayProperty):
                is_array = True
            else:
                is_array = False

            # Handle string properties that should not print ()
            if isinstance(prop, ExporterModel.StringProperty):
                is_str_prop = True
            else:
                is_str_prop = False

            # Putting the define strings into define_list
            peripheral_props.append({"region": def_region, "label": def_label, "def_value": str(def_value),
                                     "is_array": is_array, "is_str_prop": is_str_prop})
            if custom_name:
                peripheral_custom_props.append({"region": def_region, "label": custom_label, "def_value": str(def_value),
                                         "is_array": is_array, "is_str_prop": is_str_prop})

        if hasattr(prop, "inherit_options"):
            if prop.inherit_options:
                inherit_prop_list.append((prop, studio_module.ref(prop.id).get()))

    define_list += peripheral_pins
    define_list += peripheral_props
    define_list += peripheral_custom_props
    define_list += override_props

    return inherit_prop_list


def add_peripheral_defines(exp_module_list, transition, define_list, routes):
    """
    Adding defines specified in the peripheral view to define_list
    :param exp_module_list: list of ExporterModel Modules
    :param transition: chip mode change from reset to DefaultMode only (EFM32)
    :param define_list: list of dictionaries with information about defines to be generated
    :param routes: Dict containing all active pin routes with pin <-> location mapping
    :return: None
    """

    modules = {}
    inherit = []
    enabledModules = []

    # Iterating through peripheral modules
    for exp_module in exp_module_list:

        # Check if module is enabled in peripheral view
        for mod in transition.modules:
            if exp_module.name == mod.name:
                if mod.isEnabled():
                    enabledModules.append(str(mod.name))
                    modules[mod.name] = (exp_module, mod)
                    properties = exp_module.get_properties()

                    if mod.name == "PRS":
                        add_prs_defines(mod, properties, define_list, routes)
                        continue

                    # Generate enable define if property wasn't force enabled
                    if exp_module.enabled_define and mod.ref(exp_module.get_property('forceenable').id).get() != '1':
                        define_list.append({"region": mod.name, "label": exp_module.enabled_define.replace(exp_module.namespace, mod.name), "def_value": "1"})
                        define_list.append({"region": mod.name, "label": "", "def_value":"newline"})

                    # Checking if ModeProperty instance in properties
                    mode = None
                    for prop in properties:
                        if isinstance(prop, ExporterModel.ModeProperty):
                            mode = mod.ref(prop.id).get()

                    # Checking if we have a custom name
                    custom_name = None
                    for prop in properties:
                        if exp_module.has_custom_name and 'customname.STRING' in prop.id:
                            custom_name = mod.ref(prop.id).get()

                    inherit += generate_peripheral(exp_module,
                                                   mod,
                                                   define_list,
                                                   routes,
                                                   mode,
                                                   mod.name,
                                                   mod.name,
                                                   custom_name=custom_name)
                # studio module found, jumping to next exp_module
                break

    print("Enabled HWCONF modules:" + str(enabledModules))

    # Iterate through properties that inherit properties from other modules
    for prop, inherit_from in inherit:
        module = prop.parent.name
        if hasattr(prop, 'owned_mode'):
            # We are owning the other module in a specific mode
            mode = prop.owned_mode
        else:
            mode = None
        output_name = module + prop.define_name_postfix if hasattr(prop, "define_name_postfix") else module
        # Generate defines from owned module into this module, using the correct mode
        generate_peripheral(modules[inherit_from][0],
                            modules[inherit_from][1],
                            define_list,
                            routes,
                            mode,
                            module,
                            output_name,
                            prevent_override=True)


def add_prs_defines(prs_mod, properties, define_list, routes):
    def_region = prs_mod.name
    prs_properties = {}

    # Iterating through properties
    for prop in properties:
        if prop.name.isupper() or "custom_name" in prop.name:
            def_label = prop.name
            def_value = prs_mod.ref(prop.id).get()

            # Filtering out disabled/none/empty values
            if def_value == "Disabled" or def_value == "None" or def_value == "":
                continue

            # Only print  the visible properties
            if prs_mod.ref(prop.id).isHidden():
                continue

            # If property is a pin
            is_pin = False
            if isinstance(prop, ExporterModel.PinProperty):
                is_pin = True
                route = routes.get("{}_{}".format(prop.referenced_module, prop.referenced_route), None)

            # Checking if enum values have define_names - this can really be removed from prs
            if isinstance(prop, ExporterModel.EnumProperty):
                for value in prop.values.values():
                    if value.value == prs_mod.ref(prop.id).get():
                        def_value = value.define_value

            # Determining PRS channel current property is valid for
            chan = None
            if "CH" in def_label:
                chan = def_label.split("CH")[1]
            for i in range(len(chan)):
                if not chan[i].isdigit():
                    chan = chan[:i]
                    break

            # Updating property dict
            if chan not in prs_properties.keys():
                prs_properties[chan] = {"custom_name": "", "source": "", "signal": "", "pin": ""}

            if "_SIGNAL_" in def_label:
                prs_properties[chan]["signal"] = def_value
            elif "_SOURCE" in def_label:
                prs_properties[chan]["source"] = def_value
            elif "custom_name" in def_label:
                prs_properties[chan]["custom_name"] = def_value
            elif is_pin:
                prs_properties[chan]["pin"] = def_value
                prs_properties[chan]["route"] = route

    seen_common_route = False
    prs_define_prefix = 'PRS'
    for chan, options in prs_properties.items():
        if options["source"] and options["signal"]:
            define_list.append({"region": def_region, "label": "BSP_PRS_CH{}_SOURCE".format(chan), "def_value": "{}_CH_CTRL_SOURCESEL_{}".format(prs_define_prefix, options["source"])})
            define_list.append({"region": def_region, "label": "BSP_PRS_CH{}_SIGNAL".format(chan), "def_value": "{}_CH_CTRL_SIGSEL_{}".format(prs_define_prefix, options["signal"].replace("_", ""))})
            define_list.append({"region": def_region, "label": "BSP_PRS_CH{}_SOURCESIGNAL".format(chan),"def_value": "{}_{}".format(prs_define_prefix, options["signal"])})
        if options["pin"]:
            if options["route"]["common_route"]:
                define_list.append({"region": def_region, "label": "BSP_PRS_CH{}".format(chan), "pin_name": options["pin"]})
                if not seen_common_route:
                    define_list.append({"region": def_region, "label": "BSP_PRS_{}".format(options["route"]["common_route"]), "loc": options["route"]["location"]})
                    seen_common_route = True
            else:
                define_list.append({"region": def_region, "label": "BSP_PRS_CH{}".format(chan), "pin_name": options["pin"], "loc": options["route"]["location"]})

        if options["custom_name"]:
            if (options["source"] and options["signal"]) or options["pin"]:
                define_list.append({"region": def_region, "label": "", "def_value": "newline"})
            if options["source"] and options["signal"]:
                define_list.append({"region": def_region, "label": "BSP_PRS_{}_CHANNEL".format(options["custom_name"]), "def_value": chan})
                define_list.append({"region": def_region, "label": "BSP_PRS_{}_SOURCE".format(options["custom_name"]), "def_value": "{}_CH_CTRL_SOURCESEL_{}".format(prs_define_prefix, options["source"])})
                define_list.append({"region": def_region, "label": "BSP_PRS_{}_SIGNAL".format(options["custom_name"]),"def_value": "{}_CH_CTRL_SIGSEL_{}".format(prs_define_prefix, options["signal"].replace("_", ""))})
                define_list.append({"region": def_region, "label": "BSP_PRS_{}_SOURCESIGNAL".format(options["custom_name"]),"def_value": "{}_{}".format(prs_define_prefix, options["signal"])})
            if options["pin"]:
                if options["route"]["common_route"]:
                    define_list.append({"region": def_region, "label": "BSP_PRS_{}".format(options["custom_name"]), "pin_name": options["pin"]})
                else:
                    define_list.append({"region": def_region, "label": "BSP_PRS_{}".format(options["custom_name"]), "pin_name": options["pin"], "loc": options["route"]["location"]})
        if (options["source"] and options["signal"]) or options["pin"]:
            define_list.append({"region": def_region, "label": "", "def_value": "newline"})


def generate_aport_loc_define(aport_name, mod_name, mod_label, location):
    """
    Generates pin locations for APORT connected routes

    :type  aport_name: str
    :param aport_name: the routes APORT name
    :type mod_name: str
    :param mod_name: module name
    :type mod_label: str
    :param mod_label: module label
    :type location: int
    :param location: the routes pin location

    :return: The generated location
    """
    # rename aport names: AN -> 1Y
    renamed_aport_name = ''
    letter = ''
    num = aport_name[len(aport_name) - 2]

    if "IDAC" in mod_name:
        num = "1"
    if len(aport_name) > 2:
        num = "0"

    if aport_name.endswith("N"):
        letter = "Y"
    elif aport_name.endswith("P"):
        letter = "X"

    num = re.sub("A", "1", num)
    num = re.sub("B", "2", num)
    num = re.sub("C", "3", num)
    num = re.sub("D", "4", num)

    renamed_aport_name += str(num) + letter
    # generate defines
    aport_location = "_{}_".format(mod_name.rstrip('0123456789'))
    if "OPA" in mod_label:
        if "OUT" in mod_label:
            aport_location += "OPA_OUT_APORTOUTSEL_"
        else:
            aport_location += "OPA_MUX_"
    elif "ACMP" in mod_label:
        aport_location += "INPUTSEL_"
    elif "ADC" in mod_label:
        aport_location += "SINGLECTRL_"
    elif "IDAC" in mod_label:
        aport_location += "CTRL_APORTOUTSEL_"
    elif "CSEN" in mod_label:
        aport_location += "SINGLECTRL_SINGLESEL_"

    if "NEG" in mod_label:
        aport_location += "NEGSEL_"
    elif "POS" in mod_label:
        aport_location += "POSSEL_"

    aport_location += "APORT{}CH{}".format(renamed_aport_name, location)
    return aport_location


def print_defines_from_list(def_list):
    """
    Creates correct #define structures from a def_list
    :param def_list: A list containing dictionaries of defines. Each dictionary contains a region and a label,
    and one or more of the following: pin_name, location, def_value.
    :return: None
    """

    # Ensures only one of each define is generated
    index = 0
    while index < len(def_list):
        if "def_value" in def_list[index]:
            if def_list[index]["def_value"] == "newline":
                index += 1
                continue
        if def_list.count(def_list[index]) > 1:
            print("Duplicate in define list removed: " + def_list[index]["label"])
            def_list.remove(def_list[index])
            index -= 1
        index += 1

    # Find the longest label length, used to evenly space the #defines
    longest_label_length = 0
    for i in range(len(def_list)):
        if len(def_list[i]["label"]) > longest_label_length:
            longest_label_length = len(def_list[i]["label"])

    skip_prefixes = getVariable('skip_prefix')
    if not skip_prefixes:
        skip_prefixes = []

    # Print the #defines
    for define in def_list:
        region = define["region"]
        label = define["label"]
        is_array = define.get("is_array", False)
        is_str_prop = define.get("is_str_prop", False)

        # Skip defines with given prefixes
        skip = False
        for skip_prefix in skip_prefixes:
            if label.startswith(skip_prefix):
                skip = True
        if skip:
            print("skipped {}".format(label))
            continue


        # Print different types of #defines depending on the type
        if "pin_name" in define:
            pin_name = define["pin_name"]

            pin_num = str(int(pin_name[2:], 10)) + 'U'
            port = "gpioPort" + pin_name[1]

            print_define(region, label + "_PIN", pin_num, longest_label_length, is_array, is_str_prop)
            print_define(region, label + "_PORT", port, longest_label_length, is_array, is_str_prop)


        if "loc" in define:
            loc = define["loc"]
            try:
                int(loc)
                loc = str(loc) + 'U'
            except ValueError:
                pass
            print_define(region, label + "_LOC", loc, longest_label_length, is_array, is_str_prop)


        if "def_value" in define:
            def_val = define["def_value"]
            if def_val == "newline":
                newContribution(getLocation(region), label)
            else:
                print_define(region, label, def_val, longest_label_length, is_array, is_str_prop)


def print_define(region, label, define_value, longest_label_length, is_array=False, is_str_prop=False):
    """
    Prints a define in a given region, if the region does not exist it is created
    :param region: Name of region for define to go in
    :param label: name of the define
    :param define_value: value to be define
    :param longest_label_length: length of longest label, used to make even spacing
    :param is_array: bool value indicating if the define should be an array
    :param is_str_prop: bool value indicating if the define originates from a StringProperty
    :return: None
    """
    if not getLocation(region):
        startRegion(region)
        endRegion()

    # aligning the values. adding 10 to get some distance
    spaces = longest_label_length - len(label) + 10
    if is_array:
        define_value = "{{ {} }}".format(define_value)
    elif is_str_prop:
        define_value = " {}".format(define_value)
    else:
        define_value = "({})".format(define_value)
    newContribution(getLocation(region), "#define {}{}{}".format(label, " " * spaces, define_value))

def detect_framework(context):
    # First attempt to get framework from global context
    protocol_names = getVariable("protocolNames")
    if protocol_names:
        return protocol_names

    # Then attempt to get framework from project info
    proj = getProject()
    if proj:
        proj_info = getProjectInfo(proj)
        protocol_names = proj_info.get("generalProj.esf.protocolNames")
        if protocol_names:
            return protocol_names

    # Finally attempt to get framework from esf-specific context
    protocol_names = getVariable("generalProj.esf.protocolNames")
    if protocol_names:
        return protocol_names

    # Didn't find a framework
    return None

def refresh_studio_modules(framework, exp_module_list):
    """
    Ensures all studio modules associated with hwconf modules are added/removed
    :param framework: framework specifier, should match halconfig_types.Framework
    :param exp_module_list: list of ExporterModel Modules
    """
    # If the context contains a project model, use that one. In the case where
    # we are passed a project model, we don't need to update the project.
    proj_needs_update = False
    proj_model = getVariable("projectModel")
    if proj_model == None:
        proj_model = getProjectModel()
        proj_needs_update = True

    newModules = []
    print("framework:{}".format(framework if framework else "none"))
    for transition in modeTransitions():
        # Iterating through peripheral modules
        for exp_module in exp_module_list:
            # Check if module is enabled in peripheral view
            for mod in transition.modules:
                # Match module name
                if exp_module.name != mod.name:
                    continue
                # Only work with enabled modules with associated studio modules
                if not mod.isEnabled():
                    continue
                if exp_module.studio_module == None:
                    continue
                moduleId = exp_module.studio_module.getModuleId(framework)
                if moduleId:
                    newModules.append(moduleId)

    removeModules(proj_model, "SDK.HAL.*")

    print("Adding modules:" + str(newModules))
    for mod in newModules:
        addModules(proj_model, mod)

    if proj_needs_update:
        updateProjectFromModel(proj_model)