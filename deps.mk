LIBS = pebble
LINKLIBS = pebble

$(eval $(call dep_hook,pm_imx8mq_drv,$(LIBS)))
