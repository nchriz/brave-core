// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

import * as React from 'react'

import {
  SettingsMenu,
  SettingsTitle,
  SettingsWrapper,
  SettingsSidebar,
  SettingsFeatureBody,
  SettingsContent,
  SettingsCloseIcon,
  SettingsSidebarButton,
  SettingsSidebarActiveButtonSlider,
  SettingsSidebarButtonText,
  SettingsSidebarSVGContent
} from '../../components/default'

import { getLocale } from '../../../common/locale'

// Icons
import { CloseStrokeIcon } from 'brave-ui/components/icons'
import BackgroundImageIcon from './settings/icons/backgroundImage.svg'
import NraveStatsIcon from './settings/icons/braveStats.svg'
import BraveRewardsIcon from './settings/icons/braveRewards.svg'
import TopSitesIcon from './settings/icons/topSites.svg'
import ClockIcon from './settings/icons/clock.svg'
import MoreCardsIcon from './settings/icons/moreCards.svg'

// Tabs
import BackgroundImageSettings from './settings/backgroundImage'
import BraveStatsSettings from './settings/braveStats'
import BraveRewardsSettings from './settings/braveRewards'
import TopSitesSettings from './settings/topSites'
import ClockSettings from './settings/clock'
import MoreCardsSettings from './settings/moreCards'

export interface Props {
  textDirection: string
  showSettingsMenu: boolean
  onClickOutside: () => void
  toggleShowBackgroundImage: () => void
  toggleShowClock: () => void
  toggleShowStats: () => void
  toggleShowTopSites: () => void
  toggleShowRewards: () => void
  toggleShowBinance: () => void
  toggleBrandedWallpaperOptIn: () => void
  showBackgroundImage: boolean
  showStats: boolean
  showClock: boolean
  showTopSites: boolean
  brandedWallpaperOptIn: boolean
  allowSponsoredWallpaperUI: boolean
  showRewards: boolean
  showBinance: boolean
  binanceSupported: boolean
}

type ActiveTabType = 'backgroundImage' | 'braveStats' | 'braveRewards' | 'topSites' | 'clock' | 'moreCards'

interface State {
  activeTab: number
}

export default class Settings extends React.PureComponent<Props, State> {
  settingsMenuRef: React.RefObject<any>
  constructor (props: Props) {
    super(props)
    this.settingsMenuRef = React.createRef()
    this.state = { activeTab: 0 }
  }

  handleClickOutside = (event: Event) => {
    if (
      this.settingsMenuRef &&
      this.settingsMenuRef.current &&
      !this.settingsMenuRef.current.contains(event.target)
    ) {
      this.props.onClickOutside()
    }
  }

  componentDidMount () {
    document.addEventListener('mousedown', this.handleClickOutside)
    document.addEventListener('keydown', this.onKeyPressSettings)
  }

  componentWillUnmount () {
    document.removeEventListener('mousedown', this.handleClickOutside)
  }

  onKeyPressSettings = (event: KeyboardEvent) => {
    if (event.key === 'Escape') {
      this.props.onClickOutside()
    }
  }

  toggleShowBackgroundImage = () => {
    this.props.toggleShowBackgroundImage()
  }

  setActiveTab (activeTab: number) {
    this.setState({ activeTab })
  }

  get activeTabOptions (): ActiveTabType[] {
    return [
      'backgroundImage', 'braveStats', 'braveRewards', 'topSites', 'clock', 'moreCards'
    ]
  }

  getTabIcon (activeTab: number) {
    switch (activeTab) {
      case 0:
        return <SettingsSidebarSVGContent src={BackgroundImageIcon} />
      case 1:
        return <SettingsSidebarSVGContent src={NraveStatsIcon} />
      case 2:
        return <SettingsSidebarSVGContent src={BraveRewardsIcon} />
      case 3:
        return <SettingsSidebarSVGContent src={TopSitesIcon} />
      case 4:
        return <SettingsSidebarSVGContent src={ClockIcon} />
      case 5:
        return <SettingsSidebarSVGContent src={MoreCardsIcon} />
      default:
        return <SettingsSidebarSVGContent src={BackgroundImageIcon} />
    }
  }

  render () {
    const {
      textDirection,
      showSettingsMenu,
      toggleShowClock,
      toggleShowStats,
      toggleShowTopSites,
      toggleShowRewards,
      toggleBrandedWallpaperOptIn,
      showBackgroundImage,
      showStats,
      showClock,
      showTopSites,
      showRewards,
      brandedWallpaperOptIn,
      allowSponsoredWallpaperUI,
      toggleShowBinance,
      showBinance,
      binanceSupported
    } = this.props
    const { activeTab } = this.state

    return showSettingsMenu
      ? (
        <SettingsWrapper
          textDirection={textDirection}
          title={getLocale('dashboardSettingsTitle')}
        >
          <SettingsMenu innerRef={this.settingsMenuRef} textDirection={textDirection}>
            <SettingsTitle id='settingsTitle'>
              <h1>{getLocale('dashboardSettingsTitle')}</h1>
              <SettingsCloseIcon onClick={this.props.onClickOutside}>
                <CloseStrokeIcon />
              </SettingsCloseIcon>
            </SettingsTitle>
            <SettingsContent id='settingsBody'>
              <SettingsSidebar id='sidebar'>
                <SettingsSidebarActiveButtonSlider translateTo={activeTab} />
                {
                  this.activeTabOptions.map((tabName, index) => {
                    return (
                      <SettingsSidebarButton
                        key={`sidebar-button-${index}`}
                        activeTab={activeTab === index}
                        onClick={this.setActiveTab.bind(this, index)}
                      >
                        {this.getTabIcon(index)}
                        <SettingsSidebarButtonText>
                          {getLocale(tabName)}
                        </SettingsSidebarButtonText>
                      </SettingsSidebarButton>
                    )
                  })
                }
              </SettingsSidebar>
              <SettingsFeatureBody id='content'>
                {
                  activeTab === 0
                    ? (
                    <BackgroundImageSettings
                      toggleBrandedWallpaperOptIn={toggleBrandedWallpaperOptIn}
                      toggleShowBackgroundImage={this.toggleShowBackgroundImage}
                      allowSponsoredWallpaperUI={allowSponsoredWallpaperUI}
                      brandedWallpaperOptIn={brandedWallpaperOptIn}
                      showBackgroundImage={showBackgroundImage}
                    />
                  ) : null
                }
                {
                  activeTab === 1
                    ? (
                      <BraveStatsSettings
                        toggleShowStats={toggleShowStats}
                        showStats={showStats}
                      />
                    ) : null
                }
                {
                  activeTab === 2
                    ? (
                      <BraveRewardsSettings
                        toggleShowRewards={toggleShowRewards}
                        showRewards={showRewards}
                      />
                    ) : null
                }
                {
                  activeTab === 3
                    ? (
                      <TopSitesSettings
                        toggleShowTopSites={toggleShowTopSites}
                        showTopSites={showTopSites}
                      />
                    ) : null
                }
                {
                  activeTab === 4
                    ? (
                      <ClockSettings
                        toggleShowClock={toggleShowClock}
                        showClock={showClock}
                      />
                    ) : null
                }
                {
                  activeTab === 5
                    ? (
                      <MoreCardsSettings
                        toggleShowBinance={toggleShowBinance}
                        showBinance={showBinance}
                        binanceSupported={binanceSupported}
                      />
                    ) : null
                }
              </SettingsFeatureBody>
            </SettingsContent>
          </SettingsMenu>
        </SettingsWrapper>
    ) : null
  }
}
