
/*!
 * \file 	sccp_featureButton.h
 * \brief 	SCCP FeatureButton Header
 * \author 	Marcello Ceschia <marcello [at] ceschia.de>
 * \note        This program is free software and may be modified and distributed under the terms of the GNU Public License.
 *		See the LICENSE file at the top of the source tree.
 * \since 	2009-06-15
 *
 * $Date: 2012-01-02 13:56:08 +0000 (Mon, 02 Jan 2012) $
 * $Revision: 3147 $  
 */

#    ifndef SCCP_FEATUREBUTTON_H_
#define SCCP_FEATUREBUTTON_H_

void sccp_featButton_changed(sccp_device_t * device, sccp_feature_type_t featureType);

#ifdef CS_DEVSTATE_FEATURE
void sccp_devstateFeatureState_cb(const struct ast_event *ast_event, void *data);
#endif

#    endif										/* SCCP_FEATUREBUTTON_H_ */
