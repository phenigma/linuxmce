<?php
/**
 * @var \App\View\AppView $this
 * @var \Cake\Datasource\EntityInterface $entertainarea
 */
?>
<nav class="large-3 medium-4 columns" id="actions-sidebar">
    <ul class="side-nav">
        <li class="heading"><?= __('Actions') ?></li>
        <li><?= $this->Html->link(__('Edit Entertainarea'), ['action' => 'edit', $entertainarea->PK_EntertainArea]) ?> </li>
        <li><?= $this->Form->postLink(__('Delete Entertainarea'), ['action' => 'delete', $entertainarea->PK_EntertainArea], ['confirm' => __('Are you sure you want to delete # {0}?', $entertainarea->PK_EntertainArea)]) ?> </li>
        <li><?= $this->Html->link(__('List Entertainarea'), ['action' => 'index']) ?> </li>
        <li><?= $this->Html->link(__('New Entertainarea'), ['action' => 'add']) ?> </li>
        <li><?= $this->Html->link(__('List Room'), ['controller' => 'Rooms', 'action' => 'index']) ?> </li>
        <li><?= $this->Html->link(__('New Room'), ['controller' => 'Rooms', 'action' => 'add']) ?> </li>
        <li><?= $this->Html->link(__('List Command Group'), ['controller' => 'Commandgroup', 'action' => 'index']) ?> </li>
        <li><?= $this->Html->link(__('New Command Group'), ['controller' => 'Commandgroup', 'action' => 'add']) ?> </li>
    </ul>
</nav>
<div class="entertainarea view large-9 medium-8 columns content">
    <h3><?= h($entertainarea->PK_EntertainArea) ?></h3>
    <table class="vertical-table">
        <tr>
            <th scope="row"><?= __('Room') ?></th>
            <td><?= $entertainarea->has('room') ? $this->Html->link($entertainarea->room->PK_Room, ['controller' => 'Rooms', 'action' => 'view', $entertainarea->room->PK_Room]) : '' ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Description') ?></th>
            <td><?= h($entertainarea->Description) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('PK EntertainArea') ?></th>
            <td><?= $this->Number->format($entertainarea->PK_EntertainArea) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Private') ?></th>
            <td><?= $this->Number->format($entertainarea->Private) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('FK FloorplanObjectType') ?></th>
            <td><?= $this->Number->format($entertainarea->FK_FloorplanObjectType) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Id') ?></th>
            <td><?= $this->Number->format($entertainarea->psc_id) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Batch') ?></th>
            <td><?= $this->Number->format($entertainarea->psc_batch) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc User') ?></th>
            <td><?= $this->Number->format($entertainarea->psc_user) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Restrict') ?></th>
            <td><?= $this->Number->format($entertainarea->psc_restrict) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Mod') ?></th>
            <td><?= h($entertainarea->psc_mod) ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Only1Stream') ?></th>
            <td><?= $entertainarea->Only1Stream ? __('Yes') : __('No'); ?></td>
        </tr>
        <tr>
            <th scope="row"><?= __('Psc Frozen') ?></th>
            <td><?= $entertainarea->psc_frozen ? __('Yes') : __('No'); ?></td>
        </tr>
    </table>
    <div class="row">
        <h4><?= __('FloorplanInfo') ?></h4>
        <?= $this->Text->autoParagraph(h($entertainarea->FloorplanInfo)); ?>
    </div>
    <div class="related">
        <h4><?= __('Related Commandgroup') ?></h4>
        <?php if (!empty($entertainarea->command_group)): ?>
        <table cellpadding="0" cellspacing="0">
            <tr>
                <th scope="col"><?= __('PK CommandGroup') ?></th>
                <th scope="col"><?= __('FK Array') ?></th>
                <th scope="col"><?= __('FK Installation') ?></th>
                <th scope="col"><?= __('Description') ?></th>
                <th scope="col"><?= __('Hint') ?></th>
                <th scope="col"><?= __('CanTurnOff') ?></th>
                <th scope="col"><?= __('AlwaysShow') ?></th>
                <th scope="col"><?= __('CanBeHidden') ?></th>
                <th scope="col"><?= __('FK Criteria Orbiter') ?></th>
                <th scope="col"><?= __('FK DesignObj') ?></th>
                <th scope="col"><?= __('FK Template') ?></th>
                <th scope="col"><?= __('AltID') ?></th>
                <th scope="col"><?= __('FK Icon') ?></th>
                <th scope="col"><?= __('AutoGeneratedDate') ?></th>
                <th scope="col"><?= __('Disabled') ?></th>
                <th scope="col"><?= __('TemplateParm1') ?></th>
                <th scope="col"><?= __('TemplateParm2') ?></th>
                <th scope="col"><?= __('FK Text') ?></th>
                <th scope="col"><?= __('Psc Id') ?></th>
                <th scope="col"><?= __('Psc Batch') ?></th>
                <th scope="col"><?= __('Psc User') ?></th>
                <th scope="col"><?= __('Psc Frozen') ?></th>
                <th scope="col"><?= __('Psc Mod') ?></th>
                <th scope="col"><?= __('Psc Restrict') ?></th>
                <th scope="col" class="actions"><?= __('Actions') ?></th>
            </tr>
            <?php foreach ($entertainarea->command_group as $commandGroup): ?>
            <tr>
                <td><?= h($commandGroup->PK_CommandGroup) ?></td>
                <td><?= h($commandGroup->FK_Array) ?></td>
                <td><?= h($commandGroup->FK_Installation) ?></td>
                <td><?= h($commandGroup->Description) ?></td>
                <td><?= h($commandGroup->Hint) ?></td>
                <td><?= h($commandGroup->CanTurnOff) ?></td>
                <td><?= h($commandGroup->AlwaysShow) ?></td>
                <td><?= h($commandGroup->CanBeHidden) ?></td>
                <td><?= h($commandGroup->FK_Criteria_Orbiter) ?></td>
                <td><?= h($commandGroup->FK_DesignObj) ?></td>
                <td><?= h($commandGroup->FK_Template) ?></td>
                <td><?= h($commandGroup->AltID) ?></td>
                <td><?= h($commandGroup->FK_Icon) ?></td>
                <td><?= h($commandGroup->AutoGeneratedDate) ?></td>
                <td><?= h($commandGroup->Disabled) ?></td>
                <td><?= h($commandGroup->TemplateParm1) ?></td>
                <td><?= h($commandGroup->TemplateParm2) ?></td>
                <td><?= h($commandGroup->FK_Text) ?></td>
                <td><?= h($commandGroup->psc_id) ?></td>
                <td><?= h($commandGroup->psc_batch) ?></td>
                <td><?= h($commandGroup->psc_user) ?></td>
                <td><?= h($commandGroup->psc_frozen) ?></td>
                <td><?= h($commandGroup->psc_mod) ?></td>
                <td><?= h($commandGroup->psc_restrict) ?></td>
                <td class="actions">
                    <?= $this->Html->link(__('View'), ['controller' => 'Commandgroup', 'action' => 'view', $commandGroup->PK_CommandGroup]) ?>
                    <?= $this->Html->link(__('Edit'), ['controller' => 'Commandgroup', 'action' => 'edit', $commandGroup->PK_CommandGroup]) ?>
                    <?= $this->Form->postLink(__('Delete'), ['controller' => 'Commandgroup', 'action' => 'delete', $commandGroup->PK_CommandGroup], ['confirm' => __('Are you sure you want to delete # {0}?', $commandGroup->PK_CommandGroup)]) ?>
                </td>
            </tr>
            <?php endforeach; ?>
        </table>
        <?php endif; ?>
    </div>
</div>
